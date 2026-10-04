# 06 - Indices vs Pointers & Flattened ASTs

## Theory: Andrew Kelley's Data-Oriented AST Architecture

In his presentation *"Practical Data-Oriented Design"*, Andrew Kelley (creator of Zig) describes a radical optimization for compilers: **abandoning pointer-based Abstract Syntax Trees (ASTs) in favor of contiguous flattened arrays and 32-bit indices**.

### Problems with Traditional Object-Oriented ASTs:
```c
struct ASTNode {
    NodeType type;          // 4 bytes
    int32_t val;            // 4 bytes
    struct ASTNode *left;   // 8-byte 64-bit pointer
    struct ASTNode *right;  // 8-byte 64-bit pointer
}; // 24 bytes (or 32 bytes with alignment)
```
1. **64-bit Pointer Bloat:** 16 bytes (66% of the struct) are pointers just pointing to other memory addresses.
2. **Heap Fragmentation:** Every node is an individual `malloc` call, scattering nodes randomly across DRAM.
3. **Pointer Chasing:** Traversing the AST causes repeated cache misses and CPU stalls.

---

### The Data-Oriented Solution:
1. **Contiguous Flat Buffer (Post-Order / Bytecode-like layout):**
   ```c
   struct CompactASTNode {
       uint8_t op;     // 1 byte
       int32_t val;    // 4 bytes
   }; // 8 bytes total
   ```
2. **Benefits:**
   - 66% reduction in memory size (from 24B down to 8B).
   - Evaluation is a linear scan with a local stack: zero recursion, zero pointer dereferences, 100% hardware prefetcher friendly.

---

## Files in this Section

1. `ast_tree.c`: Benchmark evaluating 60,000 expression trees comparing recursive pointer AST vs flattened contiguous array.
2. `exercise.c`: Exercise implementing post-order serialization and linear stack evaluation.

---

## Running the Benchmarks

```bash
# Compile and run AST benchmark
clang -O3 ast_tree.c -o ast_tree && ./ast_tree

# Compile and run the exercise
clang -O3 exercise.c -o exercise && ./exercise
```
