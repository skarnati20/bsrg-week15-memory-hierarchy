#include "exercise.h"
#include <stdlib.h>

/*
 * =============================================================================
 * EXERCISE 02: Pointer Chasing - Contiguous Ring Buffer Queue
 * =============================================================================
 * 
 * PROBLEM:
 * A traditional dynamic linked list queue allocates a new node on every push()
 * using malloc() and frees it on pop(). This causes:
 * 1. Severe heap fragmentation
 * 2. DRAM latency stalls on every node pointer traversal
 * 3. 64-bit pointer overhead (8 bytes of pointer per 4-byte payload)
 *
 * TASK:
 * Implement a contiguous Array Ring Buffer queue.
 *
 * REQUIREMENTS:
 * 1. queue_init(): Allocate a flat array of 'capacity' int32_t elements.
 * 2. queue_push(): Place value at q->buffer[q->tail], increment tail modulo capacity.
 * 3. queue_pop(): Retrieve value at q->buffer[q->head], increment head modulo capacity.
 * 4. queue_free(): Free the allocated buffer.
 * 5. Target speedup: >= 4.0x faster than dynamic linked-list queue.
 */

void queue_init(ContiguousQueue *q, size_t capacity) {
    /* TODO: Allocate contiguous buffer and initialize indices */
    (void)q;
    (void)capacity;
}

void queue_free(ContiguousQueue *q) {
    /* TODO: Free the buffer */
    (void)q;
}

void queue_push(ContiguousQueue *q, int32_t val) {
    /* TODO: Store 'val' in buffer at tail, advance tail circularly */
    (void)q;
    (void)val;
}

int32_t queue_pop(ContiguousQueue *q) {
    /* TODO: Retrieve value at head, advance head circularly, return value */
    (void)q;
    return -1;
}
