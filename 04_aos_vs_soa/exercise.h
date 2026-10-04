#ifndef EXERCISE_04_H
#define EXERCISE_04_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    float *x;
    float *y;
    float *z;
    float *vx;
    float *vy;
    float *vz;
} ParticleSoA;

void particle_soa_init(ParticleSoA *p, size_t count);
void particle_soa_free(ParticleSoA *p);
void particle_soa_update(ParticleSoA *p, size_t count, float dt);

#endif // EXERCISE_04_H
