# 03 - Memory Layout & Struct Packing

## Theory: Alignment and Padding Holes

In C and C++, the compiler must align every struct field to a multiple of its size:
- `uint8_t` / `char` / `bool` (1 byte): aligned to 1-byte boundary
- `uint16_t` / `short` (2 bytes): aligned to 2-byte boundary
- `uint32_t` / `int` / `float` (4 bytes): aligned to 4-byte boundary
- `uint64_t` / `double` / `pointers` (8 bytes): aligned to 8-byte boundary

When fields are placed in arbitrary order, the compiler inserts **invisible padding bytes** to satisfy alignment rules.

### Bad Struct Ordering (32 Bytes total, 17 bytes wasted):
```
Offset  0: [ flag_a (1B) ]
Offset  1: [ PADDING HOLE (7B) ] <--- Wasted space
Offset  8: [ id (8B) ]
Offset 16: [ flag_b (1B) ]
Offset 17: [ PADDING HOLE (3B) ] <--- Wasted space
Offset 20: [ count (4B) ]
Offset 24: [ flag_c (1B) ]
Offset 25: [ TAIL PADDING (7B) ] <--- Wasted space
```
**Problem:** In a 64-byte L1 cache line, only **2** `BadStruct` instances fit!

---

### Andrew Kelley's Rule of Thumb (Data-Oriented Design)
Sort struct fields by alignment requirements in descending order:
1. **8-byte types:** `uint64_t`, `double`, pointers
2. **4-byte types:** `uint32_t`, `int`, `float`
3. **2-byte types:** `uint16_t`, `short`
4. **1-byte types:** `uint8_t`, `char`, `bool`

### Good Struct Ordering (16 Bytes total, 1 byte tail padding):
```
Offset  0: [ id (8B) ]
Offset  8: [ count (4B) ]
Offset 12: [ flag_a (1B) ]
Offset 13: [ flag_b (1B) ]
Offset 14: [ flag_c (1B) ]
Offset 15: [ TAIL PADDING (1B) ]
```
**Result:** **4** `GoodStruct` instances fit into a 64-byte cache line (2x cache density!).

---

## Files in this Section

1. `struct_packing.c`: Visual offset and padding inspector + memory throughput benchmark.
2. `exercise.c`: Exercise reordering a 48-byte struct to 32 bytes.

---

## Running the Benchmarks

```bash
# Compile and inspect struct layouts
clang -O3 struct_packing.c -o struct_packing && ./struct_packing

# Compile and run the exercise
clang -O3 exercise.c -o exercise && ./exercise
```
