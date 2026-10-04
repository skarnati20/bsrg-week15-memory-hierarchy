#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../common/harness.h"
#include "exercise.h"

#define NUM_TREES 50000
#define DEPTH 5 // 63 nodes per tree
#define RUNS 5

static int64_t eval_pointer_tree(const PointerNode *n) {
    if (!n) return 0;
    if (n->op == OP_CONST) return n->val;
    int64_t l = eval_pointer_tree(n->left);
    int64_t r = eval_pointer_tree(n->right);
    if (n->op == OP_ADD) return l + r;
    if (n->op == OP_SUB) return l - r;
    return (l * r) & 0xFF;
}

static PointerNode *build_tree(int d, int *counter) {
    PointerNode *n = (PointerNode *)malloc(sizeof(PointerNode));
    if (d <= 0) {
        n->op = OP_CONST;
        n->val = (++(*counter)) % 10 + 1;
        n->left = n->right = NULL;
        return n;
    }
    n->op = (d % 2 == 0) ? OP_ADD : OP_SUB;
    n->val = 0;
    n->left = build_tree(d - 1, counter);
    n->right = build_tree(d - 1, counter);
    return n;
}

int main(void) {
    print_test_banner(6, "Flattened AST & Compact Memory Layout");
    size_t nodes_per_tree = (1 << (DEPTH + 1)) - 1;
    size_t total_nodes = NUM_TREES * nodes_per_tree;

    printf("  Testing %d expression trees (%zu total AST nodes)\n", NUM_TREES, total_nodes);
    printf("  PointerNode size : %2zu bytes (Working set: %.2f MB)\n",
           sizeof(PointerNode), (double)(total_nodes * sizeof(PointerNode)) / (1024.0 * 1024.0));
    printf("  CompactNode size : %2zu bytes  (Working set: %.2f MB)\n\n",
           sizeof(CompactNode), (double)(total_nodes * sizeof(CompactNode)) / (1024.0 * 1024.0));

    PointerNode **ptr_trees = (PointerNode **)malloc(NUM_TREES * sizeof(PointerNode *));
    CompactNode *arena = (CompactNode *)aligned_alloc_64(total_nodes * sizeof(CompactNode));

    int val = 0;
    size_t flat_idx = 0;
    for (int i = 0; i < NUM_TREES; i++) {
        ptr_trees[i] = build_tree(DEPTH, &val);
        flatten_tree(ptr_trees[i], arena, &flat_idx);
    }

    if (flat_idx != total_nodes) {
        print_fail("flatten_tree() did not serialize the expected number of nodes!");
        printf("    Expected %zu nodes, Got %zu nodes\n", total_nodes, flat_idx);
        aligned_free_64(arena);
        return 1;
    }

    // Verify first 100 trees
    bool passed = true;
    for (int i = 0; i < 100; i++) {
        int64_t expected = eval_pointer_tree(ptr_trees[i]);
        int64_t actual = eval_compact_ast(&arena[i * nodes_per_tree], nodes_per_tree);
        if (expected != actual) {
            print_fail("eval_compact_ast() result mismatch!");
            printf("    Tree %d: Expected %lld, Got %lld\n", i, (long long)expected, (long long)actual);
            passed = false;
            break;
        }
    }

    if (!passed) {
        aligned_free_64(arena);
        return 1;
    }

    print_pass("Correctness verified: Flat post-order AST evaluated accurately!");

    // Benchmark Pointer AST
    uint64_t t0 = timer_now_ns();
    int64_t ptr_sum = 0;
    for (int r = 0; r < RUNS; r++) {
        for (int i = 0; i < NUM_TREES; i++) {
            ptr_sum += eval_pointer_tree(ptr_trees[i]);
        }
    }
    uint64_t t1 = timer_now_ns();
    double ptr_ms = ns_to_ms(t1 - t0) / RUNS;
    do_not_optimize(&ptr_sum);

    // Benchmark Compact AST
    uint64_t t2 = timer_now_ns();
    int64_t compact_sum = 0;
    for (int r = 0; r < RUNS; r++) {
        for (int i = 0; i < NUM_TREES; i++) {
            compact_sum += eval_compact_ast(&arena[i * nodes_per_tree], nodes_per_tree);
        }
    }
    uint64_t t3 = timer_now_ns();
    double compact_ms = ns_to_ms(t3 - t2) / RUNS;
    do_not_optimize(&compact_sum);

    print_perf_comparison("Pointer AST (Recursive)", ptr_ms, "Flattened Compact AST", compact_ms, 1.5);

    aligned_free_64(arena);
    return 0;
}
