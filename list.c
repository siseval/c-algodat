#include "list.h"


static bool list_realloc(struct list* list_ptr, size_t size)
{
    void** tmp = realloc(list_ptr->data, size); 
    if (!tmp)
    {
        fprintf(stderr, "list_realloc: realloc failed\n");
        free(list_ptr->data);
        free(list_ptr);
        return false;
    }
    list_ptr->data = tmp;
    return true;
}

struct list* list_create(const size_t size, const enum type data_type)
{
    struct list* list_ptr = malloc(sizeof(struct list));
    if (!list_ptr) 
    { 
        fprintf(stderr, "list_create: list malloc failed\n");
        return NULL; 
    }

    list_ptr->size = size;
    list_ptr->data_size = sizeof(void*);
    list_ptr->data_type = data_type;
    list_ptr->count = 0;
    list_ptr->data = malloc((1 + size) * list_ptr->data_size);

    if (!list_ptr->data) 
    {
        fprintf(stderr, "list_create: data malloc failed\n");
        free(list_ptr);
        return NULL;
    }

    return list_ptr;
}

void list_destroy(struct list* list_ptr)
{
    free(list_ptr->data);
    free(list_ptr);
}

void list_append(struct list* list_ptr, void* data_ptr)
{
    if (list_ptr->count >= list_ptr->size)
    {
        if (!list_realloc(list_ptr, list_ptr->data_size + (list_ptr->size * 2) * list_ptr->data_size))
        {
            return; 
        }
        list_ptr->size *= 2;
    }
    list_ptr->data[1 + list_ptr->count] = data_ptr;
    list_ptr->count++;
}

void list_insert(struct list* list_ptr, void* data_ptr, const int index)
{
    if (list_ptr->count >= list_ptr->size)
    {
        if (!list_realloc(list_ptr, list_ptr->data_size + (list_ptr->size * 2) * list_ptr->data_size))
        {
            return; 
        }
        list_ptr->size *= 2;
    }

    for (int i = 1 + list_ptr->count; i > 1 + index; i--)
    {
        list_ptr->data[i] = list_ptr->data[i - 1];    
    }
    list_ptr->data[1 + index] = data_ptr;
    list_ptr->count++;
}

void* list_remove(struct list* list_ptr, const int index)
{
    if (index >= list_ptr->count)
    {
        fprintf(stderr, "list_remove: index out of bounds\n");
        return NULL;
    }
    void* data_buf = list_ptr->data[index];

    for (int i = index + 2; i <= list_ptr->count; i++)
    {
        list_ptr->data[(i - 1)] = list_ptr->data[i];
    }

    list_ptr->data[1 + list_ptr->count] = NULL;
    list_ptr->count--;

    return data_buf;
}

void list_clear(struct list* list_ptr)
{
    list_ptr->count = 0;
}

void* list_get(struct list* list_ptr, const int index)
{
    if (index >= list_ptr->count)
    {
        fprintf(stderr, "list_get: index out of bounds\n");
        return NULL;
    }
    return list_ptr->data[1 + index];
}

bool list_contains(struct list* list_ptr, const void* data_ptr)
{
    for (int i = 0; i < list_ptr->count; i++)
    {
        if (list_ptr->data[1 + i] == data_ptr)
        {
            return true;
        }
    }
    return false;
}
