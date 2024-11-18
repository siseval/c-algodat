#include "queue.h"


struct queue* queue_create(uint64_t size)
{
    struct queue* queue = malloc(sizeof(struct queue));

    queue->count = 0;
    queue->size = size;
    queue->list = list_create(size);

    return queue;
}

void queue_destroy(struct queue* queue)
{
    list_destroy(queue->list);
    free(queue);
}


void queue_enqueue(struct queue* queue, void* data)
{
    list_append(queue->list, data);
    queue->count++;
}

void* queue_dequeue(struct queue* queue)
{
    if (queue->count <= 0)
    {
        fprintf(stderr, "queue_pop: queue is empty\n");
        return NULL;
    }
    queue->count--;
    return list_remove_at(queue->list, 0);
}

void* queue_peek(struct queue* queue)
{
    return list_get(queue->list, 0);
}
