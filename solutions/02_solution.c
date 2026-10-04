#include "../02_pointer_chasing/exercise.h"
#include <stdlib.h>

void queue_init(ContiguousQueue *q, size_t capacity) {
    q->buffer = (int32_t *)malloc(capacity * sizeof(int32_t));
    q->capacity = capacity;
    q->head = 0;
    q->tail = 0;
    q->count = 0;
}

void queue_free(ContiguousQueue *q) {
    free(q->buffer);
    q->buffer = NULL;
}

void queue_push(ContiguousQueue *q, int32_t val) {
    q->buffer[q->tail] = val;
    q->tail++;
    if (q->tail == q->capacity) q->tail = 0;
    q->count++;
}

int32_t queue_pop(ContiguousQueue *q) {
    int32_t val = q->buffer[q->head];
    q->head++;
    if (q->head == q->capacity) q->head = 0;
    q->count--;
    return val;
}
