#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../common/harness.h"
#include "exercise.h"

#define NUM_OPS 1000000
#define RUNS 5

void ref_queue_init(ContiguousQueue *q, size_t capacity);
void ref_queue_free(ContiguousQueue *q);
void ref_queue_push(ContiguousQueue *q, int32_t val);
int32_t ref_queue_pop(ContiguousQueue *q);

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
    print_compare_banner(2, "Pointer Chasing: Your Solution vs Reference Solution");

    // Test user correctness
    ContiguousQueue user_q;
    queue_init(&user_q, 100);
    for (int i = 0; i < 50; i++) {
        queue_push(&user_q, i * 3);
    }
    bool passed = true;
    for (int i = 0; i < 50; i++) {
        int32_t val = queue_pop(&user_q);
        if (val != i * 3) {
            print_fail("Your queue_pop returned incorrect value!");
            printf("    Expected %d, Got %d\n", i * 3, val);
            passed = false;
            break;
        }
    }
    queue_free(&user_q);

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

    // Benchmark User Queue
    uint64_t t2 = timer_now_ns();
    int64_t user_sum = 0;
    for (int r = 0; r < RUNS; r++) {
        ContiguousQueue q;
        queue_init(&q, NUM_OPS);
        for (int i = 0; i < NUM_OPS; i++) {
            queue_push(&q, i);
        }
        for (int i = 0; i < NUM_OPS; i++) {
            user_sum += queue_pop(&q);
        }
        queue_free(&q);
    }
    uint64_t t3 = timer_now_ns();
    double user_ms = ns_to_ms(t3 - t2) / RUNS;
    do_not_optimize(&user_sum);

    // Benchmark Reference Queue
    uint64_t t4 = timer_now_ns();
    int64_t ref_sum = 0;
    for (int r = 0; r < RUNS; r++) {
        ContiguousQueue q;
        ref_queue_init(&q, NUM_OPS);
        for (int i = 0; i < NUM_OPS; i++) {
            ref_queue_push(&q, i);
        }
        for (int i = 0; i < NUM_OPS; i++) {
            ref_sum += ref_queue_pop(&q);
        }
        ref_queue_free(&q);
    }
    uint64_t t5 = timer_now_ns();
    double ref_ms = ns_to_ms(t5 - t4) / RUNS;
    do_not_optimize(&ref_sum);

    print_3way_comparison("Baseline (Linked List)", ll_ms,
                          "Your Solution (exercise.c)", user_ms,
                          "Reference Ring Buffer", ref_ms,
                          2.0);
    return 0;
}
