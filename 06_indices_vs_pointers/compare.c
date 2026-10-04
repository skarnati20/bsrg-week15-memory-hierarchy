#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../common/harness.h"
#include "exercise.h"

#define NUM_TREES 50000
#define DEPTH 5 // 63 nodes per tree
#define RUNS 5

void ref_flatten_tree(const PointerNode *root, CompactNode *out_arena, size_t *out_count);
int64_t ref_eval_compact_ast(const CompactNode *nodes, size_t count);

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
    print_compare_banner(6, "Flattened AST: Your Solution vs Reference Solution");
    size_t nodes_per_tree = (1 << (DEPTH + 1)) - 1;
    size_t total_nodes = NUM_TREES * nodes_per_tree;

    printf("  Testing %d expression trees (%zu total AST nodes)\n\n", NUM_TREES, total_nodes);

    PointerNode **ptr_trees = (PointerNode **)malloc(NUM_TREES * sizeof(PointerNode *));
    CompactNode *user_arena = (CompactNode *)aligned_alloc_64(total_nodes * sizeof(CompactNode));
    CompactNode *ref_arena = (CompactNode *)aligned_alloc_64(total_nodes * sizeof(CompactNode));

    int val1 = 0;
    size_t user_flat_idx = 0;
    for (int i = 0; i < NUM_TREES; i++) {
        ptr_trees[i] = build_tree(DEPTH, &val1);
        flatten_tree(ptr_trees[i], user_arena, &user_flat_idx);
    }

    size_t ref_flat_idx = 0;
    for (int i = 0; i < NUM_TREES; i++) {
        ref_flatten_tree(ptr_trees[i], ref_arena, &ref_flat_idx);
    }

    if (user_flat_idx != total_nodes) {
        print_fail("flatten_tree() did not serialize the expected number of nodes!");
        printf("    Expected %zu nodes, Got %zu nodes\n", total_nodes, user_flat_idx);
        aligned_free_64(user_arena);
        aligned_free_64(ref_arena);
        return 1;
    }

    // Verify first 100 trees
    bool passed = true;
    for (int i = 0; i < 100; i++) {
        int64_t expected = eval_pointer_tree(ptr_trees[i]);
        int64_t actual = eval_compact_ast(&user_arena[i * nodes_per_tree], nodes_per_tree);
        if (expected != actual) {
            print_fail("eval_compact_ast() result mismatch!");
            printf("    Tree %d: Expected %lld, Got %lld\n", i, (long long)expected, (long long)actual);
            passed = false;
            break;
        }
    }

    if (!passed) {
        aligned_free_64(user_arena);
        aligned_free_64(ref_arena);
        return 1;
    }

    print_pass("Correctness verified: Flat post-order AST evaluated accurately!");

    // Benchmark Pointer AST Baseline
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

    // Benchmark User Compact AST
    uint64_t t2 = timer_now_ns();
    int64_t user_sum = 0;
    for (int r = 0; r < RUNS; r++) {
        for (int i = 0; i < NUM_TREES; i++) {
            user_sum += eval_compact_ast(&user_arena[i * nodes_per_tree], nodes_per_tree);
        }
    }
    uint64_t t3 = timer_now_ns();
    double user_ms = ns_to_ms(t3 - t2) / RUNS;
    do_not_optimize(&user_sum);

    // Benchmark Reference Compact AST
    uint64_t t4 = timer_now_ns();
    int64_t ref_sum = 0;
    for (int r = 0; r < RUNS; r++) {
        for (int i = 0; i < NUM_TREES; i++) {
            ref_sum += ref_eval_compact_ast(&ref_arena[i * nodes_per_tree], nodes_per_tree);
        }
    }
    uint64_t t5 = timer_now_ns();
    double ref_ms = ns_to_ms(t5 - t4) / RUNS;
    do_not_optimize(&ref_sum);

    print_3way_comparison("Baseline (Pointer AST)", ptr_ms,
                          "Your Solution (exercise.c)", user_ms,
                          "Reference Compact AST", ref_ms,
                          1.5);

    aligned_free_64(user_arena);
    aligned_free_64(ref_arena);
    return 0;
}
