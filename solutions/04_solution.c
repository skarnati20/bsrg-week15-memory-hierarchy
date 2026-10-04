#include "../04_aos_vs_soa/exercise.h"
#include "../common/benchmark.h"

void particle_soa_init(ParticleSoA *p, size_t count) {
    p->x  = (float *)aligned_alloc_64(count * sizeof(float));
    p->y  = (float *)aligned_alloc_64(count * sizeof(float));
    p->z  = (float *)aligned_alloc_64(count * sizeof(float));
    p->vx = (float *)aligned_alloc_64(count * sizeof(float));
    p->vy = (float *)aligned_alloc_64(count * sizeof(float));
    p->vz = (float *)aligned_alloc_64(count * sizeof(float));
}

void particle_soa_free(ParticleSoA *p) {
    aligned_free_64(p->x);
    aligned_free_64(p->y);
    aligned_free_64(p->z);
    aligned_free_64(p->vx);
    aligned_free_64(p->vy);
    aligned_free_64(p->vz);
}

void particle_soa_update(ParticleSoA *p, size_t count, float dt) {
    float * restrict px = p->x;
    float * restrict py = p->y;
    float * restrict pz = p->z;
    const float * restrict pvx = p->vx;
    const float * restrict pvy = p->vy;
    const float * restrict pvz = p->vz;

    for (size_t i = 0; i < count; i++) {
        px[i] += pvx[i] * dt;
        py[i] += pvy[i] * dt;
        pz[i] += pvz[i] * dt;
    }
}
