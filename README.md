# Cache Locality & Data-Oriented Design (DoD) in C

An interactive, hands-on suite of exercises and benchmarks demonstrating the impact of **CPU cache hierarchy**, **spatial/temporal locality**, and **memory layout optimizations** (such as those championed by Andrew Kelley in his famous *Practical Data-Oriented Design* talk).

---

## 🚀 Quick Start

```bash
# 1. Run all standalone demonstrations & benchmarks:
make bench

# 2. Run the automated test harness against your exercise files:
make exercises

# 3. Test an individual exercise (e.g. Exercise 1):
make test1

# 4. Run the test harness against the reference solutions:
make solutions
```

---

## 🧠 Why Memory Layout Matters

Modern CPU cores run at **~3–5 GHz**, capable of executing multiple arithmetic instructions per nanosecond. However, accessing main memory (DRAM) takes **50–100 nanoseconds (~200–300 clock cycles)**. 

To hide this latency, CPUs use a hierarchy of fast caches:

| Level | Size | Latency | Bandwidth |
|---|---|---|---|
| **L1 Data Cache** | ~32 KB – 128 KB | ~1 ns (4 cycles) | ~100+ GB/s |
| **L2 Cache** | ~512 KB – 4 MB | ~3–4 ns (12–14 cycles) | ~50+ GB/s |
| **L3 Cache** | ~16 MB – 64 MB | ~10–20 ns (40–60 cycles) | ~30+ GB/s |
| **Main RAM (DRAM)** | 8 GB – 64 GB | ~50–100 ns (200–300 cycles!) | ~15–25 GB/s |

### The 64-Byte Cache Line
CPUs **never** read single bytes from RAM. Whenever data is requested, the CPU loads a contiguous **64-byte block (Cache Line)** into the cache:

```
Address: [ 0x00 ........................................ 0x3F ]
         └─────── 64 Bytes fetched into L1 Cache Line ────────┘
```
- **Spatial Locality:** If you read byte 0, bytes 1..63 are already in L1 cache for free!
- **Data-Oriented Design (DoD):** Arrange data so that **100% of the bytes loaded into each 64-byte cache line are actively used** by your loops.

---

## 🎯 The Exercises

All exercise implementation details and benchmarking boilerplate are hidden inside automated test harnesses. As the implementer, you only need to edit the clean `exercise.h` / `exercise.c` file in each directory.

### Exercise 01: Spatial Locality (2D Filter)
- **Directory:** `01_spatial_locality/`
- **Concept:** Row-Major vs. Column-Major 2D Matrix traversal.
- **Problem:** A naive image blur filter loops column-by-column, jumping 16KB per step and thrashing cache lines.
- **Task:** Rewrite the 3x3 box blur filter to traverse memory row-by-row (`y` outer, `x` inner).
- **Run:** `make test1` (Target: **$\ge 4.0\times$ speedup**)

---

### Exercise 02: Pointer Chasing vs. Contiguous Memory
- **Directory:** `02_pointer_chasing/`
- **Concept:** Linked list pointer hopping vs contiguous array ring buffers.
- **Problem:** Dynamically-allocated linked lists scatter nodes across heap memory, stalling the CPU on DRAM latency and defeating hardware prefetchers.
- **Task:** Implement an array-based contiguous Ring Buffer Queue (`push` and `pop`).
- **Run:** `make test2` (Target: **$\ge 2.0\times$ speedup**)

---

### Exercise 03: Struct Packing & Alignment (Andrew Kelley DoD Principle)
- **Directory:** `03_struct_packing/`
- **Concept:** Eliminating invisible padding holes by ordering fields by alignment.
- **Problem:** `UnoptimizedMonster` takes 48 bytes despite having only 26 bytes of data because poorly ordered fields create interior alignment holes.
- **Task:** Reorder fields from largest alignment requirement (8-byte) down to smallest (1-byte) to pack the struct into $\le 32$ bytes.
- **Run:** `make test3` (Target: **$\le 32$ bytes, $\ge 1.15\times$ speedup**)

```
Bad Ordering (48B):  [u8][7B pad][u64][u8][1B pad][u16][4B pad][f64][u8][3B pad][u32][u8][7B pad]
Good Ordering (32B): [u64][f64][u32][u16][u8][u8][u8][u8][6B tail pad]
```

---

### Exercise 04: Array-of-Structs (AoS) vs. Struct-of-Arrays (SoA)
- **Directory:** `04_aos_vs_soa/`
- **Concept:** Cache line utilization and SIMD auto-vectorization.
- **Problem:** Updating `pos += vel * dt` on an array of 64-byte entity structs loads 40 bytes of unused strings/tags per particle into L1 cache (~60% bandwidth waste).
- **Task:** Implement a Struct-of-Arrays (SoA) layout with parallel float buffers (`x, y, z, vx, vy, vz`).
- **Run:** `make test4` (Target: **$\ge 2.5\times$ speedup**)

---

### Exercise 05: Hot / Cold Data Splitting
- **Directory:** `05_hot_cold_splitting/`
- **Concept:** Keeping per-tick simulation working sets small enough to fit inside L1/L2 caches.
- **Problem:** A monolithic 216-byte game unit struct floods caches with dialogue strings and sound paths during frame ticks.
- **Task:** Separate the 16-byte `HotUnit` state from cold metadata and implement the tick update loop.
- **Run:** `make test5` (Target: **$\ge 5.0\times$ speedup**)

---

### Exercise 06: Flattened AST & Arena Memory (Zig Compiler Technique)
- **Directory:** `06_indices_vs_pointers/`
- **Concept:** Replacing 64-bit recursive pointer trees with compact flattened post-order arrays.
- **Problem:** Traditional ASTs use two 64-bit pointers per node (16 bytes of pointers alone) and require recursive pointer-chasing traversal.
- **Task:** Implement post-order tree serialization into a flat 8-byte `CompactNode` arena and a linear stack-based evaluator.
- **Run:** `make test6` (Target: **$\ge 1.5\times$ speedup**)

---

## 📁 Repository Structure

```
.
├── Makefile                          # Unified build & test driver
├── common/
│   ├── benchmark.h                   # Monotonic timer & aligned memory helpers
│   └── harness.h                     # Test verification & reporting harness
├── 01_spatial_locality/
│   ├── exercise.h / exercise.c       # Exercise 01 (Clean student workspace)
│   ├── runner.c                      # Hidden test & benchmark harness
│   ├── row_vs_col.c                  # Standalone benchmark
│   └── stride_benchmark.c            # Cache-line striding demonstration
├── 02_pointer_chasing/
│   ├── exercise.h / exercise.c       # Exercise 02 (Queue implementation)
│   ├── runner.c                      # Hidden test & benchmark harness
│   └── list_vs_array.c               # Standalone benchmark
├── 03_struct_packing/
│   ├── exercise.h / exercise.c       # Exercise 03 (Struct layout optimization)
│   ├── runner.c                      # Hidden test & benchmark harness
│   └── struct_packing.c              # Standalone layout inspector
├── 04_aos_vs_soa/
│   ├── exercise.h / exercise.c       # Exercise 04 (SoA physics)
│   ├── runner.c                      # Hidden test & benchmark harness
│   └── aos_vs_soa.c                  # Standalone AoS vs SoA benchmark
├── 05_hot_cold_splitting/
│   ├── exercise.h / exercise.c       # Exercise 05 (Hot/Cold separation)
│   ├── runner.c                      # Hidden test & benchmark harness
│   └── hot_cold.c                    # Standalone benchmark
├── 06_indices_vs_pointers/
│   ├── exercise.h / exercise.c       # Exercise 06 (Flattened AST)
│   ├── runner.c                      # Hidden test & benchmark harness
│   └── ast_tree.c                    # Standalone benchmark
└── solutions/
    ├── 01_solution.c ... 06_solution.c  # Clean reference solutions
```
