# 02 - Pointer Chasing vs Contiguous Memory

## Theory: The Cost of Pointer Chasing

Modern CPUs have memory latencies of:
- **L1 Cache:** ~1 ns (4 clock cycles)
- **L2 Cache:** ~3–4 ns (12–14 clock cycles)
- **L3 Cache:** ~10–20 ns (40–60 clock cycles)
- **Main Memory (DRAM):** ~50–100 ns (200–300 clock cycles!)

When traversing a linked list or pointer-heavy graph:
```
[ Node A ] ---> [ Node B ] ---> [ Node C ]
 (0x1000)        (0x8A40)        (0x23F0)
```
1. The CPU **cannot know** where `Node B` is located until `Node A->next` has finished loading from memory.
2. The hardware prefetcher is rendered completely useless because pointer addresses hop arbitrarily across the heap.
3. The CPU core sits completely stalled (idle) for hundreds of cycles waiting for each node to arrive from DRAM.

In contrast, contiguous arrays (`array[i]`):
```
[ Int 0 | Int 1 | Int 2 | Int 3 | Int 4 | ... | Int 15 ] (All in 1 Cache Line!)
```
- A single 64-byte line fetch brings 16 elements at once.
- The prefetcher detects linear stride and streams memory in ahead of time.

---

## Files in this Section

1. `list_vs_array.c`: Compares flat array vs contiguous pool linked list vs shuffled heap linked list.
2. `exercise.c`: Replace a dynamically-allocated linked list queue with a contiguous ring buffer.

---

## Running the Benchmarks

```bash
# Compile and run pointer chasing benchmark
clang -O3 list_vs_array.c -o list_vs_array && ./list_vs_array

# Compile and run the exercise
clang -O3 exercise.c -o exercise && ./exercise
```
