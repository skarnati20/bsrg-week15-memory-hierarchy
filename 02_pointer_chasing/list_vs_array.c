#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "../common/benchmark.h"

#define NUM_ELEMENTS 1000000 // 1 Million elements
#define RUNS 5

typedef struct ListNode {
    int32_t value;
    int32_t padding; // Keep struct at 16 bytes (aligned with 8-byte pointer)
    struct ListNode *next;
} ListNode;

// Shuffle an array of indices using Fisher-Yates
static void shuffle(int *indices, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = indices[i];
        indices[i] = indices[j];
        indices[j] = tmp;
    }
}

int main(void) {
    print_header("02 - Pointer Chasing: Contiguous Array vs Linked List vs Random Hopping");
    printf("Number of Elements: %d (%.2f MB)\n", NUM_ELEMENTS, (double)(NUM_ELEMENTS * sizeof(ListNode)) / (1024.0 * 1024.0));
    printf("Iterations        : %d runs\n\n", RUNS);

    // 1. Contiguous Array of integers
    int32_t *flat_array = (int32_t *)aligned_alloc_64(NUM_ELEMENTS * sizeof(int32_t));
    for (int i = 0; i < NUM_ELEMENTS; i++) {
        flat_array[i] = 1;
    }

    // 2. Sequential Linked List (nodes contiguous in memory pool)
    ListNode *pool_seq = (ListNode *)aligned_alloc_64(NUM_ELEMENTS * sizeof(ListNode));
    for (int i = 0; i < NUM_ELEMENTS; i++) {
        pool_seq[i].value = 1;
        pool_seq[i].next = (i + 1 < NUM_ELEMENTS) ? &pool_seq[i + 1] : NULL;
    }

    // 3. Shuffled Linked List (nodes scattered across memory, simulating fragmented heap)
    ListNode *pool_rand = (ListNode *)aligned_alloc_64(NUM_ELEMENTS * sizeof(ListNode));
    int *indices = (int *)malloc(NUM_ELEMENTS * sizeof(int));
    for (int i = 0; i < NUM_ELEMENTS; i++) {
        indices[i] = i;
        pool_rand[i].value = 1;
        pool_rand[i].next = NULL;
    }
    shuffle(indices, NUM_ELEMENTS);
    for (int i = 0; i < NUM_ELEMENTS - 1; i++) {
        pool_rand[indices[i]].next = &pool_rand[indices[i + 1]];
    }
    pool_rand[indices[NUM_ELEMENTS - 1]].next = NULL;
    ListNode *rand_head = &pool_rand[indices[0]];

    // Benchmark 1: Flat Array
    uint64_t t0 = timer_now_ns();
    int64_t sum_array = 0;
    for (int r = 0; r < RUNS; r++) {
        for (int i = 0; i < NUM_ELEMENTS; i++) {
            sum_array += flat_array[i];
        }
    }
    uint64_t t1 = timer_now_ns();
    double array_ms = ns_to_ms(t1 - t0) / RUNS;
    do_not_optimize(&sum_array);

    // Benchmark 2: Sequential Linked List
    uint64_t t2 = timer_now_ns();
    int64_t sum_seq = 0;
    for (int r = 0; r < RUNS; r++) {
        ListNode *curr = &pool_seq[0];
        while (curr) {
            sum_seq += curr->value;
            curr = curr->next;
        }
    }
    uint64_t t3 = timer_now_ns();
    double seq_ms = ns_to_ms(t3 - t2) / RUNS;
    do_not_optimize(&sum_seq);

    // Benchmark 3: Random/Shuffled Linked List (True Pointer Chasing)
    uint64_t t4 = timer_now_ns();
    int64_t sum_rand = 0;
    for (int r = 0; r < RUNS; r++) {
        ListNode *curr = rand_head;
        while (curr) {
            sum_rand += curr->value;
            curr = curr->next;
        }
    }
    uint64_t t5 = timer_now_ns();
    double rand_ms = ns_to_ms(t5 - t4) / RUNS;
    do_not_optimize(&sum_rand);

    print_bench_result("Random Pointer Chasing (Heap List)", rand_ms, rand_ms, NUM_ELEMENTS * sizeof(ListNode));
    print_bench_result("Sequential Linked List (In-Order) ", seq_ms, rand_ms, NUM_ELEMENTS * sizeof(ListNode));
    print_bench_result("Contiguous Dense Array (Vector)   ", array_ms, rand_ms, NUM_ELEMENTS * sizeof(int32_t));

    printf("\nExplanation:\n");
    printf("  1. Random Pointer Chasing: The CPU cannot predict where 'curr->next' points.\n");
    printf("     Every node access is a cache miss and a round-trip to DRAM (~50-100ns CPU stall).\n");
    printf("  2. Sequential Linked List: Nodes happen to sit in order in memory, so cache lines\n");
    printf("     are partially reused, but 8 bytes are still wasted on pointers.\n");
    printf("  3. Contiguous Flat Array: 100%% payload density, SIMD vectorizable, and CPU hardware\n");
    printf("     stream prefetcher achieves near maximum memory bandwidth.\n");

    aligned_free_64(flat_array);
    aligned_free_64(pool_seq);
    aligned_free_64(pool_rand);
    free(indices);
    return 0;
}
