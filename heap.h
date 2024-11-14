#ifndef HEAP_H
#define HEAP_H

#include <stdlib.h>
#include <stdint.h>
#include "list.h"

#define heap(T) heap_create(32)

struct heap
{
    uint64_t count;
    uint64_t size;
    struct list* list;
};

struct heap* heap_create(uint64_t size);
void heap_destroy(struct heap* heap);

void heap_push(struct heap* heap, void* data);
void* heap_pop(struct heap* heap);
void* heap_peek(struct heap* heap);


#endif
