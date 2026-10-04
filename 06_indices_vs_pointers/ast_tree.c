#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "../common/benchmark.h"

#define NUM_TREES 60000
#define DEPTH 5 // 63 nodes per tree
#define RUNS 5

typedef enum {
    OP_ADD = 0,
    OP_SUB = 1,
    OP_MUL = 2,
    OP_CONST = 3
} OpType;

// -----------------------------------------------------------------------------
// Traditional AST Node:
// - Heap allocated via malloc
// - Contains two 64-bit pointers (16 bytes of pointers alone!)
// - Total sizeof: 24 bytes (or 32 with alignment)
// -----------------------------------------------------------------------------
typedef struct PtrNode {
    OpType op;
    int32_t val;
    struct PtrNode *left;   // 8 bytes
    struct PtrNode *right;  // 8 bytes
} PtrNode;

// -----------------------------------------------------------------------------
// Andrew Kelley / DoD Flattened AST:
// - Packed into a single contiguous array in post-order (Reverse Polish)
// - Uses 0 pointers! Only 8 bytes per node (packed struct: 1-byte op + 4-byte val)
// - Evaluated sequentially without recursion or pointer chasing.
// -----------------------------------------------------------------------------
typedef struct {
    uint8_t op;
    int32_t val;
} LinearNode;

// Recursive pointer-based tree evaluation
static int64_t eval_ptr_tree(const PtrNode *n) {
    if (!n) return 0;
    if (n->op == OP_CONST) return n->val;
    int64_t l = eval_ptr_tree(n->left);
    int64_t r = eval_ptr_tree(n->right);
    if (n->op == OP_ADD) return l + r;
    if (n->op == OP_SUB) return l - r;
    return (l * r) & 0xFF;
}

// Flat contiguous array evaluation using a small local stack
static int64_t eval_linear_ast(const LinearNode *nodes, size_t count) {
    int64_t stack[64];
    int sp = 0;
    for (size_t i = 0; i < count; i++) {
        uint8_t op = nodes[i].op;
        if (op == OP_CONST) {
            stack[sp++] = nodes[i].val;
        } else {
            int64_t r = stack[--sp];
            int64_t l = stack[--sp];
            if (op == OP_ADD) stack[sp++] = l + r;
            else if (op == OP_SUB) stack[sp++] = l - r;
            else stack[sp++] = (l * r) & 0xFF;
        }
    }
    return stack[0];
}

// Tree builder helpers
static PtrNode *build_ptr_tree(int d, int *counter) {
    PtrNode *n = (PtrNode *)malloc(sizeof(PtrNode));
    if (d <= 0) {
        n->op = OP_CONST;
        n->val = (++(*counter)) % 10 + 1;
        n->left = n->right = NULL;
        return n;
    }
    n->op = (d % 2 == 0) ? OP_ADD : OP_SUB;
    n->val = 0;
    n->left = build_ptr_tree(d - 1, counter);
    n->right = build_ptr_tree(d - 1, counter);
    return n;
}

static void flatten_to_linear(const PtrNode *n, LinearNode *out_buf, size_t *out_idx) {
    if (!n) return;
    flatten_to_linear(n->left, out_buf, out_idx);
    flatten_to_linear(n->right, out_buf, out_idx);
    size_t i = (*out_idx)++;
    out_buf[i].op = (uint8_t)n->op;
    out_buf[i].val = n->val;
}

int main(void) {
    print_header("06 - AST Memory Representation: Pointer Trees vs Flattened DoD Arrays");

    size_t nodes_per_tree = (1 << (DEPTH + 1)) - 1; // 63 nodes
    size_t total_nodes = NUM_TREES * nodes_per_tree;

    printf("Number of Expression Trees : %d\n", NUM_TREES);
    printf("Nodes Per Tree             : %zu\n", nodes_per_tree);
    printf("Total AST Nodes            : %zu\n\n", total_nodes);

    printf("Memory Footprint Comparison:\n");
    printf("  Pointer AST Node : %zu bytes (x %zu = %.2f MB)\n",
           sizeof(PtrNode), total_nodes, (double)(total_nodes * sizeof(PtrNode)) / (1024.0 * 1024.0));
    printf("  Linear AST Node  : %zu bytes  (x %zu = %.2f MB)\n\n",
           sizeof(LinearNode), total_nodes, (double)(total_nodes * sizeof(LinearNode)) / (1024.0 * 1024.0));

    PtrNode **ptr_trees = (PtrNode **)malloc(NUM_TREES * sizeof(PtrNode *));
    LinearNode *linear_arena = (LinearNode *)aligned_alloc_64(total_nodes * sizeof(LinearNode));

    int counter = 0;
    size_t linear_idx = 0;
    for (int i = 0; i < NUM_TREES; i++) {
        ptr_trees[i] = build_ptr_tree(DEPTH, &counter);
        flatten_to_linear(ptr_trees[i], linear_arena, &linear_idx);
    }

    // Benchmark Pointer Tree Evaluation
    uint64_t t0 = timer_now_ns();
    int64_t ptr_sum = 0;
    for (int r = 0; r < RUNS; r++) {
        for (int i = 0; i < NUM_TREES; i++) {
            ptr_sum += eval_ptr_tree(ptr_trees[i]);
        }
    }
    uint64_t t1 = timer_now_ns();
    double ptr_ms = ns_to_ms(t1 - t0) / RUNS;
    do_not_optimize(&ptr_sum);

    // Benchmark Flattened DoD Array Evaluation
    uint64_t t2 = timer_now_ns();
    int64_t lin_sum = 0;
    for (int r = 0; r < RUNS; r++) {
        for (int i = 0; i < NUM_TREES; i++) {
            lin_sum += eval_linear_ast(&linear_arena[i * nodes_per_tree], nodes_per_tree);
        }
    }
    uint64_t t3 = timer_now_ns();
    double lin_ms = ns_to_ms(t3 - t2) / RUNS;
    do_not_optimize(&lin_sum);

    print_bench_result("Pointer AST (Recursive Ptrs) ", ptr_ms, ptr_ms, total_nodes * sizeof(PtrNode));
    print_bench_result("Flattened AST (Contiguous DoD)", lin_ms, ptr_ms, total_nodes * sizeof(LinearNode));

    printf("\nExplanation:\n");
    printf("  1. In the traditional pointer AST, each node is an individual heap allocation.\n");
    printf("     Evaluating expressions incurs recursive calls and arbitrary pointer hops.\n");
    printf("  2. In Andrew Kelley's data-oriented design, the entire AST is serialized into\n");
    printf("     a dense contiguous buffer, reducing node size from 24B to 8B (66%% reduction)\n");
    printf("     and streaming through memory sequentially with maximum cache hit rate.\n");

    aligned_free_64(linear_arena);
    return 0;
}
