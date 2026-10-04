#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "../common/benchmark.h"

#define NUM_PARTICLES 2000000
#define ITERATIONS 10
#define DT 0.016f // 60 FPS delta time

// -----------------------------------------------------------------------------
// Array-of-Structs (AoS) Layout
// Each particle struct contains position, velocity, and unused metadata (64 bytes)
// -----------------------------------------------------------------------------
typedef struct {
    float x, y, z;          // 12 bytes - ACTIVE in physics
    float vx, vy, vz;       // 12 bytes - ACTIVE in physics
    float mass;             // 4 bytes
    uint32_t id;            // 4 bytes
    char name[24];          // 24 bytes - UNUSED in physics loop
    uint32_t color;         // 4 bytes  - UNUSED in physics loop
    uint32_t lifetime;      // 4 bytes  - UNUSED in physics loop
} ParticleAoS; // Total: 64 bytes (Exactly 1 particle per cache line)

// -----------------------------------------------------------------------------
// Struct-of-Arrays (SoA) Layout
// Separate arrays for each property. Physics loop only touches pos and vel arrays!
// -----------------------------------------------------------------------------
typedef struct {
    float *x;
    float *y;
    float *z;
    float *vx;
    float *vy;
    float *vz;
    // Cold data stored in separate buffers
    float *mass;
    uint32_t *id;
    char (*name)[24];
    uint32_t *color;
    uint32_t *lifetime;
} ParticleSoA;

// Update physics in AoS: Wastes 40 bytes out of every 64-byte cache line fetch!
static void update_physics_aos(ParticleAoS *particles, size_t count, float dt) {
    for (size_t i = 0; i < count; i++) {
        particles[i].x += particles[i].vx * dt;
        particles[i].y += particles[i].vy * dt;
        particles[i].z += particles[i].vz * dt;
    }
}

// Update physics in SoA: 100% cache line utilization + SIMD vectorizable!
static void update_physics_soa(ParticleSoA *soa, size_t count, float dt) {
    float * restrict px = soa->x;
    float * restrict py = soa->y;
    float * restrict pz = soa->z;
    const float * restrict pvx = soa->vx;
    const float * restrict pvy = soa->vy;
    const float * restrict pvz = soa->vz;

    for (size_t i = 0; i < count; i++) {
        px[i] += pvx[i] * dt;
        py[i] += pvy[i] * dt;
        pz[i] += pvz[i] * dt;
    }
}

int main(void) {
    print_header("04 - Array-of-Structs (AoS) vs Struct-of-Arrays (SoA)");
    printf("Particle Count: %d\n", NUM_PARTICLES);
    printf("Iterations    : %d physics updates\n\n", ITERATIONS);

    size_t aos_bytes = NUM_PARTICLES * sizeof(ParticleAoS);
    ParticleAoS *aos = (ParticleAoS *)aligned_alloc_64(aos_bytes);

    ParticleSoA soa;
    soa.x = (float *)aligned_alloc_64(NUM_PARTICLES * sizeof(float));
    soa.y = (float *)aligned_alloc_64(NUM_PARTICLES * sizeof(float));
    soa.z = (float *)aligned_alloc_64(NUM_PARTICLES * sizeof(float));
    soa.vx = (float *)aligned_alloc_64(NUM_PARTICLES * sizeof(float));
    soa.vy = (float *)aligned_alloc_64(NUM_PARTICLES * sizeof(float));
    soa.vz = (float *)aligned_alloc_64(NUM_PARTICLES * sizeof(float));

    for (size_t i = 0; i < NUM_PARTICLES; i++) {
        aos[i].x = (float)i;
        aos[i].y = (float)i * 2.0f;
        aos[i].z = 0.0f;
        aos[i].vx = 1.0f;
        aos[i].vy = 2.0f;
        aos[i].vz = 3.0f;

        soa.x[i] = aos[i].x;
        soa.y[i] = aos[i].y;
        soa.z[i] = aos[i].z;
        soa.vx[i] = aos[i].vx;
        soa.vy[i] = aos[i].vy;
        soa.vz[i] = aos[i].vz;
    }

    // Benchmark AoS
    uint64_t t0 = timer_now_ns();
    for (int iter = 0; iter < ITERATIONS; iter++) {
        update_physics_aos(aos, NUM_PARTICLES, DT);
    }
    uint64_t t1 = timer_now_ns();
    double aos_ms = ns_to_ms(t1 - t0) / ITERATIONS;
    do_not_optimize(aos);

    // Benchmark SoA
    uint64_t t2 = timer_now_ns();
    for (int iter = 0; iter < ITERATIONS; iter++) {
        update_physics_soa(&soa, NUM_PARTICLES, DT);
    }
    uint64_t t3 = timer_now_ns();
    double soa_ms = ns_to_ms(t3 - t2) / ITERATIONS;
    do_not_optimize(soa.x);

    // Bytes read + written in active physics fields
    size_t active_bytes_transferred = NUM_PARTICLES * sizeof(float) * 6; // 3 read, 3 read+write
    print_bench_result("Array-of-Structs (AoS) ", aos_ms, aos_ms, aos_bytes);
    print_bench_result("Struct-of-Arrays (SoA) ", soa_ms, aos_ms, active_bytes_transferred);

    printf("\nExplanation:\n");
    printf("  In AoS, each particle occupies 64 bytes. Updating position and velocity requires\n");
    printf("  fetching the ENTIRE 64-byte struct into cache, discarding 40 bytes of cold metadata.\n");
    printf("  In SoA, position and velocity are stored in contiguous float arrays.\n");
    printf("  The CPU transfers 0 wasted bytes into cache lines and auto-vectorizes loops with SIMD!\n");

    aligned_free_64(aos);
    aligned_free_64(soa.x);
    aligned_free_64(soa.y);
    aligned_free_64(soa.z);
    aligned_free_64(soa.vx);
    aligned_free_64(soa.vy);
    aligned_free_64(soa.vz);
    return 0;
}
