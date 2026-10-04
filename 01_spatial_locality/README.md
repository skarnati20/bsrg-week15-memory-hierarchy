# 01 - Spatial Locality & The 64-Byte Cache Line

## Theory: How CPU Caches Read Memory

CPUs do not read individual bytes or 4-byte integers from main memory directly. Whenever a core accesses an address, the memory controller transfers a **64-byte Cache Line** containing that address into the L1 data cache.

```
Main Memory:
[ byte 0 ... byte 63 ] -> Fetched into 1 L1 Cache Line (64 Bytes)
[ float 0, float 1, float 2, float 3, ..., float 15 ] (16 floats per line)
```

### Spatial Locality
**Spatial locality** means that if you access memory at address `A`, you will soon access addresses close to `A` (e.g., `A + 4`, `A + 8`).

- **Sequential / Row-Major Access:**
  When iterating through array `[0], [1], [2], ...`:
  - Reading `[0]` causes a cache miss -> loads 64 bytes (16 floats) into L1 cache.
  - Reading `[1]` through `[15]` are **instant L1 cache hits** (~1 nanosecond).
  - Overall: 93.75% hit rate! Plus, the CPU hardware prefetcher recognizes the linear stream and pre-loads upcoming lines before the CPU even requests them.

- **Column-Major Access:**
  When iterating down columns `[0][0]`, `[1][0]`, `[2][0]`:
  - Each step jumps by `WIDTH * 4 bytes` (e.g., 16,384 bytes).
  - Every single read fetches a new 64-byte line, uses only 4 bytes, and discards the remaining 60 bytes.
  - Result: 0% spatial cache reuse, cache thrashing, and memory bus saturation.

---

## Files in this Section

1. `row_vs_col.c`: Benchmark comparing Row-Major vs Column-Major 2D array traversal.
2. `stride_benchmark.c`: Measures nanoseconds per access as stride scales from 1 to 128 elements.
3. `exercise.c`: Practice exercise optimizing a 2D image filter from column-major to row-major.

---

## Running the Benchmarks

```bash
# Compile and run row vs column benchmark
clang -O3 row_vs_col.c -o row_vs_col && ./row_vs_col

# Compile and run stride benchmark
clang -O3 stride_benchmark.c -o stride_benchmark && ./stride_benchmark

# Compile and run the exercise
clang -O3 exercise.c -o exercise && ./exercise
```
