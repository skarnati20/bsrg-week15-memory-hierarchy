#include "exercise.h"

/*
 * =============================================================================
 * EXERCISE 06: Flattened AST & Arena Arrays (Andrew Kelley Zig Technique)
 * =============================================================================
 * 
 * PROBLEM:
 * Pointer-based Abstract Syntax Trees (ASTs) use 64-bit pointers (16 bytes per node)
 * and scatter nodes across the heap via individual malloc() calls. Traversing the
 * tree requires recursive calls and arbitrary pointer chasing across DRAM.
 *
 * TASK:
 * 1. Implement flatten_tree(): Serialize the tree into `out_arena` using
 *    POST-ORDER traversal (left subtree, right subtree, current node).
 * 2. Implement eval_compact_ast(): Evaluate the flattened post-order array linearly
 *    using a local fixed stack (e.g. int64_t stack[64], int sp = 0).
 *
 * REQUIREMENTS:
 * 1. Post-order serialization: If node is OP_CONST, write {OP_CONST, val}.
 *    If operator, serialize left, then right, then current node.
 * 2. Linear evaluation:
 *    - For OP_CONST: push val to stack.
 *    - For OP_ADD: pop right, pop left, push (left + right).
 *    - For OP_SUB: pop right, pop left, push (left - right).
 *    - For OP_MUL: pop right, pop left, push ((left * right) & 0xFF).
 * 3. Target speedup: >= 1.8x faster than recursive pointer AST.
 */

void flatten_tree(const PointerNode *root, CompactNode *out_arena, size_t *out_count) {
    /* TODO: Post-order recursive traversal writing into out_arena[*out_count] */
    (void)root;
    (void)out_arena;
    (void)out_count;
}

int64_t eval_compact_ast(const CompactNode *nodes, size_t count) {
    /* TODO: Stack-based linear evaluation loop over nodes[0..count-1] */
    (void)nodes;
    (void)count;
    return 0;
}
