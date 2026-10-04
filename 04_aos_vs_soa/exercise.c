#include "exercise.h"
#include <stdlib.h>

/*
 * =============================================================================
 * EXERCISE 04: Array-of-Structs (AoS) vs Struct-of-Arrays (SoA)
 * =============================================================================
 * 
 * PROBLEM:
 * In an Array-of-Structs layout (struct Particle { float x, y, z, vx, vy, vz; char name[32]; ... }),
 * updating position from velocity forces the CPU to load 64 bytes per particle
 * into L1 cache, discarding 40 bytes of cold metadata on every step (~60% bandwidth wasted).
 *
 * TASK:
 * Implement a Struct-of-Arrays (SoA) layout for high-throughput physics updates.
 *
 * REQUIREMENTS:
 * 1. particle_soa_init(): Allocate contiguous float arrays for x, y, z, vx, vy, vz.
 * 2. particle_soa_free(): Free the allocated arrays.
 * 3. particle_soa_update(): Update positions:
 *       x[i] += vx[i] * dt;
 *       y[i] += vy[i] * dt;
 *       z[i] += vz[i] * dt;
 * 4. Target speedup: >= 2.5x faster than Array-of-Structs baseline.
 */

void particle_soa_init(ParticleSoA *p, size_t count) {
    /* TODO: Allocate parallel float arrays of size 'count' */
    (void)p;
    (void)count;
}

void particle_soa_free(ParticleSoA *p) {
    /* TODO: Free all allocated arrays */
    (void)p;
}

void particle_soa_update(ParticleSoA *p, size_t count, float dt) {
    /* TODO: Integrate positions with velocity: p->x[i] += p->vx[i] * dt */
    (void)p;
    (void)count;
    (void)dt;
}
