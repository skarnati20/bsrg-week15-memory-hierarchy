#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "../common/harness.h"
#include "exercise.h"

#define NUM_PARTICLES 4000000
#define RUNS 5
#define DT 0.016f

typedef struct {
    float x, y, z;
    float vx, vy, vz;
    float mass;
    char name[32];
    uint32_t tag;
    uint32_t flags;
} ParticleAoS;

static void update_aos(ParticleAoS *p, size_t count, float dt) {
    for (size_t i = 0; i < count; i++) {
        p[i].x += p[i].vx * dt;
        p[i].y += p[i].vy * dt;
        p[i].z += p[i].vz * dt;
    }
}

int main(void) {
    print_test_banner(4, "Array-of-Structs (AoS) vs Struct-of-Arrays (SoA)");
    print_info("Testing 4,000,000 particles across 5 physics steps...");

    ParticleAoS *aos = (ParticleAoS *)aligned_alloc_64(NUM_PARTICLES * sizeof(ParticleAoS));
    for (size_t i = 0; i < NUM_PARTICLES; i++) {
        aos[i].x = (float)i;
        aos[i].y = (float)i * 0.5f;
        aos[i].z = (float)i * 0.1f;
        aos[i].vx = 1.0f;
        aos[i].vy = 2.0f;
        aos[i].vz = 3.0f;
    }

    ParticleSoA soa;
    memset(&soa, 0, sizeof(ParticleSoA));
    particle_soa_init(&soa, NUM_PARTICLES);

    if (!soa.x || !soa.y || !soa.z || !soa.vx || !soa.vy || !soa.vz) {
        print_fail("particle_soa_init() failed to allocate all 6 float arrays. Complete particle_soa_init() first!");
        aligned_free_64(aos);
        return 1;
    }

    for (size_t i = 0; i < NUM_PARTICLES; i++) {
        soa.x[i] = aos[i].x;
        soa.y[i] = aos[i].y;
        soa.z[i] = aos[i].z;
        soa.vx[i] = aos[i].vx;
        soa.vy[i] = aos[i].vy;
        soa.vz[i] = aos[i].vz;
    }

    // Run one update to test correctness
    update_aos(aos, NUM_PARTICLES, DT);
    particle_soa_update(&soa, NUM_PARTICLES, DT);

    bool passed = true;
    for (size_t i = 0; i < 100; i++) {
        if (fabsf(aos[i].x - soa.x[i]) > 1e-3f || fabsf(aos[i].y - soa.y[i]) > 1e-3f) {
            print_fail("Position mismatch after physics update!");
            printf("    At particle %zu: Expected x=%.3f, Got x=%.3f\n", i, aos[i].x, soa.x[i]);
            passed = false;
            break;
        }
    }

    if (!passed) {
        particle_soa_free(&soa);
        aligned_free_64(aos);
        return 1;
    }

    print_pass("Correctness verified: SoA physics integration matches AoS reference!");

    // Benchmark AoS
    uint64_t t0 = timer_now_ns();
    for (int r = 0; r < RUNS; r++) {
        update_aos(aos, NUM_PARTICLES, DT);
    }
    uint64_t t1 = timer_now_ns();
    double aos_ms = ns_to_ms(t1 - t0) / RUNS;
    do_not_optimize(aos);

    // Benchmark SoA
    uint64_t t2 = timer_now_ns();
    for (int r = 0; r < RUNS; r++) {
        particle_soa_update(&soa, NUM_PARTICLES, DT);
    }
    uint64_t t3 = timer_now_ns();
    double soa_ms = ns_to_ms(t3 - t2) / RUNS;
    do_not_optimize(soa.x);

    print_perf_comparison("Array-of-Structs (AoS)", aos_ms, "Struct-of-Arrays (SoA)", soa_ms, 2.5);

    particle_soa_free(&soa);
    aligned_free_64(aos);
    return 0;
}
