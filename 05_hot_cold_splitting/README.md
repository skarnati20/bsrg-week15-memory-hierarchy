# 05 - Hot / Cold Data Splitting

## Theory: The Working Set Problem

Most software entities contain a mix of:
- **Hot Data:** Variables read/written every frame, tick, or query (e.g., position, health, velocity, timestamps).
- **Cold Data:** Variables read only on rare events like user interaction, audio triggers, UI rendering, death, or initialization (e.g., entity name, dialogue trees, inventory icons, sound paths).

### Monolithic Entity Problem:
If you place everything into one struct:
```c
struct Entity {
    float health;           // Hot (4B)
    float cooldown;         // Hot (4B)
    char name[64];          // Cold (64B)
    char icon_path[64];     // Cold (64B)
    char sound_event[64];   // Cold (64B)
}; // Total = 200+ bytes!
```
When iterating over 1,000,000 entities to update cooldowns:
- Every entity spans **4 cache lines**.
- The CPU spends 95% of its time fetching cold strings that are never used in the loop.
- The working set is ~200 MB, blowing way past the 16–32 MB L3 cache down into slow DRAM.

---

### The Hot/Cold Split Solution:
```c
// Hot array: 8 bytes per entity -> fits 8 entities per 64-byte cache line!
// 1,000,000 entities = 8 MB (Fits completely inside L3 cache!)
struct HotEntity {
    float health;
    float cooldown;
};

// Cold array: accessed only when an entity interacts with UI or dies
struct ColdEntity {
    char name[64];
    char icon_path[64];
    char sound_event[64];
};
```

---

## Files in this Section

1. `hot_cold.c`: Benchmark showing ~12x speedup when splitting cold metadata from hot state.
2. `exercise.c`: Practice exercise splitting monolithic unit state.

---

## Running the Benchmarks

```bash
# Compile and run hot/cold benchmark
clang -O3 hot_cold.c -o hot_cold && ./hot_cold

# Compile and run the exercise
clang -O3 exercise.c -o exercise && ./exercise
```
