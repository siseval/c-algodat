#ifndef QUEUE_H
#define QUEUE_H

#include <stdlib.h>
#include <stdint.h>
#include "list.h"

#define queue(T) queue_create(32)

struct queue
{
    uint64_t count;
    uint64_t size;
    struct list* list;
};

struct queue* queue_create(uint64_t size);
void queue_destroy(struct queue* queue);

void queue_enqueue(struct queue* queue, void* data);
void* queue_dequeue(struct queue* queue);
void* queue_peek(struct queue* queue);

#endif
