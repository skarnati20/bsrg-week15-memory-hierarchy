#ifndef EXERCISE_02_H
#define EXERCISE_02_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    int32_t *buffer;
    size_t capacity;
    size_t head;
    size_t tail;
    size_t count;
} ContiguousQueue;

void queue_init(ContiguousQueue *q, size_t capacity);
void queue_free(ContiguousQueue *q);
void queue_push(ContiguousQueue *q, int32_t val);
int32_t queue_pop(ContiguousQueue *q);

#endif // EXERCISE_02_H
