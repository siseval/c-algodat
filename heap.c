#include "heap.h"


struct heap* heap_create(uint64_t size, bool takes_arrays)
{
    struct heap* heap = malloc(sizeof(struct heap));

    heap->count = 0;
    heap->size = size;
    heap->list = list_create(size);
    heap->takes_arrays = takes_arrays;

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
        uint64_t data_int_value = *(uint64_t*)(heap->takes_arrays ? data : &data);
        uint64_t parent_index = parent_index_of(i);
        void* parent_data = list_get(heap->list, parent_index);
        uint64_t parent_data_int_value = *(uint64_t*)(heap->takes_arrays ? parent_data : &parent_data);

        if (data_int_value >= parent_data_int_value)
        {
            return;
        }
        void* data_buf = data;
        list_replace(heap->list, parent_data, i);
        list_replace(heap->list, data_buf, parent_index);
        i = parent_index;
    }
}

void* heap_pop(struct heap* heap)
{
    if (heap->count <= 0)
    {
        fprintf(stderr, "heap_pop: heap is empty.\n");
        return NULL;
    }
    void* data_buf = list_get(heap->list, 0);

    if (heap->count == 1)
    {
        heap->count--;
        list_remove(heap->list, 0);
        return data_buf;
    }

    list_replace(heap->list, list_get(heap->list, heap->count - 1), 0);
    list_remove(heap->list, heap->count - 1);
    heap->count--;

    uint64_t i = 0;
    while (left_index_of(i) < heap->count)
    {
        void* data = list_get(heap->list, i);
        uint64_t data_int_value = *(uint64_t*)(heap->takes_arrays ? data : &data);

        uint64_t left_index = left_index_of(i);
        void* left_data = list_get(heap->list, left_index);
        uint64_t left_data_int_value = *(uint64_t*)(heap->takes_arrays ? left_data : &left_data);

        uint64_t lowest_data_index = left_index;
        void* lowest_data = left_data;
        uint64_t lowest_data_int_value = *(uint64_t*)(heap->takes_arrays ? lowest_data : &lowest_data);

        uint64_t right_index = right_index_of(i);
        if (right_index < heap->count)
        {
            void* right_data = list_get(heap->list, right_index);
            uint64_t right_data_int_value = *(uint64_t*)(heap->takes_arrays ? right_data : &right_data);
            lowest_data_index = left_data_int_value <= right_data_int_value ? left_index : right_index;
            lowest_data = left_data_int_value <= right_data_int_value ? left_data : right_data;
            lowest_data_int_value = left_data_int_value <= right_data_int_value ? left_data_int_value : right_data_int_value;
        }

        if (data_int_value <= lowest_data_int_value)
        {
            return data_buf;
        }
        void* lowest_data_buf = lowest_data;
        list_replace(heap->list, data, lowest_data_index);
        list_replace(heap->list, lowest_data_buf, i);
        i = lowest_data_index;
    }

    return data_buf;
}

void* heap_peek(struct heap* heap)
{
    return list_get(heap->list, 0);
}

