#ifndef STACK_H
#define STACK_H

#include <stdlib.h>
#include <stdint.h>
#include "list.h"

#define stack(T) stack_create(32)

struct stack
{
    uint64_t count;
    uint64_t size;
    struct list* list;
};

struct stack* stack_create(uint64_t size);
void stack_destroy(struct stack* stack);

void stack_push(struct stack* stack, void* data);
void* stack_pop(struct stack* stack);
void* stack_peek(struct stack* stack);

#endif
