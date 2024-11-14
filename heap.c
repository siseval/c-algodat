#include "heap.h"


struct heap* heap_create(uint64_t size)
{
    struct heap* heap = malloc(sizeof(struct heap));

    heap->count = 0;
    heap->size = size;
    heap->list = list_create(size);

    return heap;
}

void heap_destroy(struct heap* heap)
{
    list_destroy(heap->list);
    free(heap);
}


static uint64_t parent_index_of(const uint64_t index)
{
    return (index - 1) / 2;
}

static uint64_t left_index_of(const uint64_t index)
{
    return index * 2 + 1;
}

static uint64_t right_index_of(const uint64_t index)
{
    return index * 2 + 2;
}


void heap_push(struct heap* heap, void* data)
{
    list_append(heap->list, data);
    heap->count++;

    uint64_t i = heap->count - 1;
    while (i > 0)
    {
        void* data = list_get(heap->list, i);
        uint64_t parent_index = parent_index_of(i);
        void* parent_data = list_get(heap->list, parent_index);

        if (data < parent_data)
        {
            void* data_buf = data;
            list_replace(heap->list, parent_data, i);
            list_replace(heap->list, data_buf, parent_index);
            i = parent_index;
            continue;
        }
        return;
    }
}

void* heap_pop(struct heap* heap)
{
    void* data_buf = list_get(heap->list, 0);

    if (heap->count == 1)
    {
        heap->count--;
        return data_buf;
    }

    list_replace(heap->list, list_get(heap->list, heap->count - 1), 0);
    list_remove(heap->list, heap->count - 1);
    heap->count--;

    uint64_t i = 0;
    while (left_index_of(i) < heap->count - 1)
    {
        void* data = list_get(heap->list, i);

        uint64_t left_index = left_index_of(i);
        uint64_t right_index = right_index_of(i);
        void* left_data = list_get(heap->list, left_index);
        void* right_data = list_get(heap->list, right_index);
        
        void* lowest_data = left_data <= right_data ? left_data : right_data;
        uint64_t lowest_data_index = left_data <= right_data ? left_index : right_index;

        if (data > lowest_data)
        {
            void* data_buf = data;
            list_replace(heap->list, lowest_data, i);
            list_replace(heap->list, data_buf, lowest_data_index);
            i = lowest_data_index;
            continue;
        }
        break;
    }

    return data_buf;
}

void* heap_peek(struct heap* heap)
{
    return list_get(heap->list, 0);
}

