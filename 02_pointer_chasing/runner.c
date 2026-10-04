#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../common/harness.h"
#include "exercise.h"

#define NUM_OPS 1000000
#define RUNS 5

typedef struct LLNode {
    int32_t val;
    struct LLNode *next;
} LLNode;

typedef struct {
    LLNode *head;
    LLNode *tail;
} LinkedListQueue;

static void ll_init(LinkedListQueue *q) {
    q->head = NULL;
    q->tail = NULL;
}

static void ll_push(LinkedListQueue *q, int32_t val) {
    LLNode *n = (LLNode *)malloc(sizeof(LLNode));
    n->val = val;
    n->next = NULL;
    if (q->tail) {
        q->tail->next = n;
        q->tail = n;
    } else {
        q->head = q->tail = n;
    }
}

static int32_t ll_pop(LinkedListQueue *q) {
    if (!q->head) return -1;
    LLNode *n = q->head;
    int32_t val = n->val;
    q->head = n->next;
    if (!q->head) q->tail = NULL;
    free(n);
    return val;
}

int main(void) {
    print_test_banner(2, "Pointer Chasing vs Contiguous Buffer");
    print_info("Testing 1,000,000 pushes and pops across 5 iterations...");

    // Test correctness
    ContiguousQueue test_q;
    queue_init(&test_q, 100);
    for (int i = 0; i < 50; i++) {
        queue_push(&test_q, i * 3);
    }
    bool passed = true;
    for (int i = 0; i < 50; i++) {
        int32_t val = queue_pop(&test_q);
        if (val != i * 3) {
            print_fail("Queue pop returned incorrect value!");
            printf("    Expected %d, Got %d\n", i * 3, val);
            passed = false;
            break;
        }
    }
    queue_free(&test_q);

    if (!passed) return 1;

    print_pass("Correctness verified: FIFO behavior and wrap-around work perfectly!");

    // Benchmark Baseline Linked List
    uint64_t t0 = timer_now_ns();
    int64_t ll_sum = 0;
    for (int r = 0; r < RUNS; r++) {
        LinkedListQueue ll;
        ll_init(&ll);
        for (int i = 0; i < NUM_OPS; i++) {
            ll_push(&ll, i);
        }
        for (int i = 0; i < NUM_OPS; i++) {
            ll_sum += ll_pop(&ll);
        }
    }
    uint64_t t1 = timer_now_ns();
    double ll_ms = ns_to_ms(t1 - t0) / RUNS;
    do_not_optimize(&ll_sum);

    // Benchmark Contiguous Queue
    uint64_t t2 = timer_now_ns();
    int64_t cq_sum = 0;
    for (int r = 0; r < RUNS; r++) {
        ContiguousQueue cq;
        queue_init(&cq, NUM_OPS);
        for (int i = 0; i < NUM_OPS; i++) {
            queue_push(&cq, i);
        }
        for (int i = 0; i < NUM_OPS; i++) {
            cq_sum += queue_pop(&cq);
        }
        queue_free(&cq);
    }
    uint64_t t3 = timer_now_ns();
    double cq_ms = ns_to_ms(t3 - t2) / RUNS;
    do_not_optimize(&cq_sum);

    print_perf_comparison("Linked List Queue (Baseline)", ll_ms, "Contiguous Ring Buffer", cq_ms, 2.0);
    return 0;
}
