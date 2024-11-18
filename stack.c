#include "stack.h"


struct stack* stack_create(uint64_t size)
{
    struct stack* stack = malloc(sizeof(struct stack));

    stack->count = 0;
    stack->size = size;
    stack->list = list_create(size);

    return stack;
}

void stack_destroy(struct stack* stack)
{
    list_destroy(stack->list);
    free(stack);
}


void stack_push(struct stack* stack, void* data)
{
    list_append(stack->list, data);
    stack->count++;
}

void* stack_pop(struct stack* stack)
{
    if (stack->count <= 0)
    {
        fprintf(stderr, "stack_pop: stack is empty\n");
        return NULL;
    }
    stack->count--;
    return list_remove_at(stack->list, stack->count);
}

void* stack_peek(struct stack* stack)
{
    return list_get(stack->list, stack->count - 1);
}
