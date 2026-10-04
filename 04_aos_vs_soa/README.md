# 04 - Array-of-Structs (AoS) vs Struct-of-Arrays (SoA)

## Theory: Cache Line Utilization in Data-Oriented Design

In Object-Oriented Design (OOD), programmers naturally organize data around entities:
```c
struct Entity {
    float x, y, z;       // Active in physics update
    float vx, vy, vz;    // Active in physics update
    char name[32];       // Unused during physics
    uint32_t health;     // Unused during physics
    uint32_t inventory;  // Unused during physics
};
```

When an array of entities `Entity entities[N]` (Array-of-Structs) is looped over to update physics:
```c
for (int i = 0; i < N; i++) {
    entities[i].x += entities[i].vx * dt;
}
```
### What happens at the hardware level?
1. The CPU fetches a **64-byte Cache Line**.
2. It uses only the 24 bytes of position and velocity.
3. The remaining 40 bytes (strings, health, inventory) are loaded into L1 cache, displacing other useful data, and never read!
4. **Bandwidth Efficiency:** Only ~37% of memory bus traffic is actually used.

---

### The Data-Oriented Design (SoA) Solution:
Break up the struct into parallel arrays:
```c
struct EntitySoA {
    float *x, *y, *z;
    float *vx, *vy, *vz;
    // Cold arrays allocated separately
    char (*name)[32];
    uint32_t *health;
};
```
Now, iterating through `x` and `vx`:
- Every single byte fetched into the 64-byte cache line is 100% useful data.
- The CPU can issue packed SIMD (128-bit/256-bit) vector instructions to process 4 to 8 particles per clock cycle.

---

## Files in this Section

1. `aos_vs_soa.c`: Complete benchmark demonstrating 2.8x+ speedup of SoA over AoS.
2. `exercise.c`: Transform a particle physics engine from AoS to SoA.

---

## Running the Benchmarks

```bash
# Compile and run AoS vs SoA benchmark
clang -O3 aos_vs_soa.c -o aos_vs_soa && ./aos_vs_soa

# Compile and run the exercise
clang -O3 exercise.c -o exercise && ./exercise
```
