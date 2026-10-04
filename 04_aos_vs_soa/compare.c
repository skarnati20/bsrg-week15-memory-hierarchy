#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "../common/harness.h"
#include "exercise.h"

#define NUM_PARTICLES 4000000
#define RUNS 5
#define DT 0.016f

void ref_particle_soa_init(ParticleSoA *p, size_t count);
void ref_particle_soa_free(ParticleSoA *p);
void ref_particle_soa_update(ParticleSoA *p, size_t count, float dt);

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
    print_compare_banner(4, "AoS vs SoA: Your Solution vs Reference Solution");

    ParticleAoS *aos = (ParticleAoS *)aligned_alloc_64(NUM_PARTICLES * sizeof(ParticleAoS));
    for (size_t i = 0; i < NUM_PARTICLES; i++) {
        aos[i].x = (float)i;
        aos[i].y = (float)i * 0.5f;
        aos[i].z = (float)i * 0.1f;
        aos[i].vx = 1.0f;
        aos[i].vy = 2.0f;
        aos[i].vz = 3.0f;
    }

    ParticleSoA user_soa;
    memset(&user_soa, 0, sizeof(ParticleSoA));
    particle_soa_init(&user_soa, NUM_PARTICLES);

    if (!user_soa.x || !user_soa.y || !user_soa.z || !user_soa.vx || !user_soa.vy || !user_soa.vz) {
        print_fail("particle_soa_init() failed to allocate arrays. Complete particle_soa_init() first!");
        aligned_free_64(aos);
        return 1;
    }

    ParticleSoA ref_soa;
    ref_particle_soa_init(&ref_soa, NUM_PARTICLES);

    for (size_t i = 0; i < NUM_PARTICLES; i++) {
        user_soa.x[i] = aos[i].x;
        user_soa.y[i] = aos[i].y;
        user_soa.z[i] = aos[i].z;
        user_soa.vx[i] = aos[i].vx;
        user_soa.vy[i] = aos[i].vy;
        user_soa.vz[i] = aos[i].vz;

        ref_soa.x[i] = aos[i].x;
        ref_soa.y[i] = aos[i].y;
        ref_soa.z[i] = aos[i].z;
        ref_soa.vx[i] = aos[i].vx;
        ref_soa.vy[i] = aos[i].vy;
        ref_soa.vz[i] = aos[i].vz;
    }

    // Run one update to verify correctness
    update_aos(aos, NUM_PARTICLES, DT);
    particle_soa_update(&user_soa, NUM_PARTICLES, DT);
    ref_particle_soa_update(&ref_soa, NUM_PARTICLES, DT);

    bool passed = true;
    for (size_t i = 0; i < 100; i++) {
        if (fabsf(ref_soa.x[i] - user_soa.x[i]) > 1e-3f || fabsf(ref_soa.y[i] - user_soa.y[i]) > 1e-3f) {
            print_fail("Position mismatch between your SoA and reference SoA!");
            printf("    At particle %zu: Ref x=%.3f, Yours x=%.3f\n", i, ref_soa.x[i], user_soa.x[i]);
            passed = false;
            break;
        }
    }

    if (!passed) {
        particle_soa_free(&user_soa);
        ref_particle_soa_free(&ref_soa);
        aligned_free_64(aos);
        return 1;
    }

    print_pass("Correctness verified: SoA physics integration matches reference!");

    // Benchmark AoS Baseline
    uint64_t t0 = timer_now_ns();
    for (int r = 0; r < RUNS; r++) {
        update_aos(aos, NUM_PARTICLES, DT);
    }
    uint64_t t1 = timer_now_ns();
    double aos_ms = ns_to_ms(t1 - t0) / RUNS;
    do_not_optimize(aos);

    // Benchmark User SoA
    uint64_t t2 = timer_now_ns();
    for (int r = 0; r < RUNS; r++) {
        particle_soa_update(&user_soa, NUM_PARTICLES, DT);
    }
    uint64_t t3 = timer_now_ns();
    double user_ms = ns_to_ms(t3 - t2) / RUNS;
    do_not_optimize(user_soa.x);

    // Benchmark Reference SoA
    uint64_t t4 = timer_now_ns();
    for (int r = 0; r < RUNS; r++) {
        ref_particle_soa_update(&ref_soa, NUM_PARTICLES, DT);
    }
    uint64_t t5 = timer_now_ns();
    double ref_ms = ns_to_ms(t5 - t4) / RUNS;
    do_not_optimize(ref_soa.x);

    print_3way_comparison("Baseline (Array-of-Structs)", aos_ms,
                          "Your Solution (exercise.c)", user_ms,
                          "Reference SoA", ref_ms,
                          2.5);

    particle_soa_free(&user_soa);
    ref_particle_soa_free(&ref_soa);
    aligned_free_64(aos);
    return 0;
}
