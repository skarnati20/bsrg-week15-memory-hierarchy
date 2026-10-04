# Cache Locality & Data-Oriented Design (DoD) in C

An interactive, hands-on suite of exercises and benchmarks demonstrating the impact of **CPU cache hierarchy**, **spatial/temporal locality**, and **memory layout optimizations** (such as those championed by Andrew Kelley in his famous *Practical Data-Oriented Design* talk).

---

## Ὠ0 Quick Start

### 1. Test Single Problems / Exercises
```bash
make test1      # Test Exercise 01 (Spatial Locality)
make test2      # Test Exercise 02 (Pointer Chasing)
make test3      # Test Exercise 03 (Struct Packing)
make test4      # Test Exercise 04 (AoS vs SoA)
make test5      # Test Exercise 05 (Hot/Cold Splitting)
make test6      # Test Exercise 06 (Flattened AST)

make exercises  # Test all exercises in sequence
```

### 2. Compare Your Solution Directly Against Reference (3-Way Benchmark)
```bash
make compare1   # 3-Way benchmark for Exercise 1 (Baseline vs Yours vs Reference)
make compare2   # 3-Way benchmark for Exercise 2
make compare3   # 3-Way benchmark for Exercise 3
make compare4   # 3-Way benchmark for Exercise 4
make compare5   # 3-Way benchmark for Exercise 5
make compare6   # 3-Way benchmark for Exercise 6

make compare    # Run all 3-way comparisons
```

### 3. Demonstrations & Reference Solutions
```bash
make bench      # Run standalone demonstrations of memory and cache effects
make demo1..6   # Run individual demonstration for a topic (e.g. make demo1)
make solutions  # Verify all official reference solutions
```

---

## ᾞ0 Why Memory Layout Matters

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

## ἺF The Exercises

All exercise test harnesses and benchmarking boilerplate are decoupled and hidden. You only edit the clean `exercise.h` / `exercise.c` file in each folder.

### Exercise 01: Spatial Locality (2D Filter)
- **Directory:** `01_spatial_locality/`
- **Concept:** Row-Major vs. Column-Major 2D Matrix traversal.
- **Problem:** A naive image blur filter loops column-by-column, jumping 16KB per step and thrashing cache lines.
- **Task:** Rewrite the 3x3 box blur filter to traverse memory row-by-row (`y` outer, `x` inner).
- **Test:** `make test1` | **Compare:** `make compare1` (Target: **$\ge 4.0\times$ speedup**)

---

### Exercise 02: Pointer Chasing vs. Contiguous Memory
- **Directory:** `02_pointer_chasing/`
- **Concept:** Linked list pointer hopping vs contiguous array ring buffers.
- **Problem:** Dynamically-allocated linked lists scatter nodes across heap memory, stalling the CPU on DRAM latency and defeating hardware prefetchers.
- **Task:** Implement an array-based contiguous Ring Buffer Queue (`push` and `pop`).
- **Test:** `make test2` | **Compare:** `make compare2` (Target: **$\ge 2.0\times$ speedup**)

---

### Exercise 03: Struct Packing & Alignment (Andrew Kelley DoD Principle)
- **Directory:** `03_struct_packing/`
- **Concept:** Eliminating invisible padding holes by ordering fields by alignment.
- **Problem:** `UnoptimizedMonster` takes 48 bytes despite having only 26 bytes of data because poorly ordered fields create interior alignment holes.
- **Task:** Reorder fields from largest alignment requirement (8-byte) down to smallest (1-byte) in `exercise.h` to pack the struct into $\le 32$ bytes.
- **Test:** `make test3` | **Compare:** `make compare3` (Target: **$\le 32$ bytes, $\ge 1.15\times$ speedup**)

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
- **Test:** `make test4` | **Compare:** `make compare4` (Target: **$\ge 2.5\times$ speedup**)

---

### Exercise 05: Hot / Cold Data Splitting
- **Directory:** `05_hot_cold_splitting/`
- **Concept:** Keeping per-tick simulation working sets small enough to fit inside L1/L2 caches.
- **Problem:** A monolithic 216-byte game unit struct floods caches with dialogue strings and sound paths during frame ticks.
- **Task:** Separate the 16-byte `HotUnit` state from cold metadata and implement the tick update loop.
- **Test:** `make test5` | **Compare:** `make compare5` (Target: **$\ge 5.0\times$ speedup**)

---

### Exercise 06: Flattened AST & Arena Memory (Zig Compiler Technique)
- **Directory:** `06_indices_vs_pointers/`
- **Concept:** Replacing 64-bit recursive pointer trees with compact flattened post-order arrays.
- **Problem:** Traditional ASTs use two 64-bit pointers per node (16 bytes of pointers alone) and require recursive pointer-chasing traversal.
- **Task:** Implement post-order tree serialization into a flat 8-byte `CompactNode` arena and a linear stack-based evaluator.
- **Test:** `make test6` | **Compare:** `make compare6` (Target: **$\ge 1.5\times$ speedup**)

---

## Ὄ1 Repository Structure

```
.
├── Makefile                          # Unified build, test, and 3-way compare driver
├── .gitignore                        # Git exclusion rules
├── common/
│   ├── benchmark.h                   # Monotonic timer & aligned memory helpers
│   └── harness.h                     # Test verification & 3-way benchmark reporter
├── 01_spatial_locality/
│   ├── exercise.h / exercise.c       # Clean student workspace (Row-major filter)
│   ├── runner.c                      # Single-problem test harness
│   ├── compare.c                     # 3-Way comparison harness (Baseline vs Yours vs Ref)
│   ├── row_vs_col.c                  # Standalone benchmark
│   └── stride_benchmark.c            # Cache-line striding demonstration
├── 02_pointer_chasing/
│   ├── exercise.h / exercise.c       # Clean student workspace (Queue implementation)
│   ├── runner.c                      # Single-problem test harness
│   ├── compare.c                     # 3-Way comparison harness
│   └── list_vs_array.c               # Standalone benchmark
├── 03_struct_packing/
│   ├── exercise.h / exercise.c       # Clean student workspace (Struct layout optimization)
│   ├── runner.c                      # Single-problem test harness
│   ├── compare.c                     # 3-Way comparison harness
│   └── struct_packing.c              # Standalone layout inspector
├── 04_aos_vs_soa/
│   ├── exercise.h / exercise.c       # Clean student workspace (SoA physics)
│   ├── runner.c                      # Single-problem test harness
│   ├── compare.c                     # 3-Way comparison harness
│   └── aos_vs_soa.c                  # Standalone AoS vs SoA benchmark
├── 05_hot_cold_splitting/
│   ├── exercise.h / exercise.c       # Clean student workspace (Hot/Cold separation)
│   ├── runner.c                      # Single-problem test harness
│   ├── compare.c                     # 3-Way comparison harness
│   └── hot_cold.c                    # Standalone benchmark
├── 06_indices_vs_pointers/
│   ├── exercise.h / exercise.c       # Clean student workspace (Flattened AST)
│   ├── runner.c                      # Single-problem test harness
│   ├── compare.c                     # 3-Way comparison harness
│   └── ast_tree.c                    # Standalone benchmark
└── solutions/
    ├── 01_solution.c ... 06_solution.c  # Reference implementations
```
