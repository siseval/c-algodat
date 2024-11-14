#include "list.h"


static bool list_realloc(struct list* list, uint64_t size)
{
    void** tmp = realloc(list->data, size); 
    if (!tmp)
    {
        fprintf(stderr, "list_realloc: realloc failed\n");
        free(list->data);
        free(list);
        return false;
    }
    list->data = tmp;
    return true;
}

struct list* list_create(const uint64_t size)
{
    struct list* list = calloc(1, sizeof(struct list));
    if (!list) 
    { 
        fprintf(stderr, "list_create: list malloc failed\n");
        return NULL; 
    }

    list->size = size;
    list->data_size = sizeof(void*);
    list->count = 0;
    list->data = calloc((1 + size), list->data_size);

    if (!list->data) 
    {
        fprintf(stderr, "list_create: data malloc failed\n");
        free(list);
        return NULL;
    }

    return list;
}

void list_destroy(struct list* list)
{
    free(list->data);
    free(list);
}

void list_append(struct list* list, void* data)
{
    if (list->count >= list->size)
    {
        if (!list_realloc(list, list->data_size + (list->size * 2) * list->data_size))
        {
            return; 
        }
        list->size *= 2;
    }
    list->data[1 + list->count] = data;
    list->count++;
}

void list_insert(struct list* list, void* data, const uint64_t index)
{
    if (list->count >= list->size)
    {
        if (!list_realloc(list, list->data_size + (list->size * 2) * list->data_size))
        {
            return; 
        }
        list->size *= 2;
    }

    for (int i = 1 + list->count; i > 1 + index; i--)
    {
        list->data[i] = list->data[i - 1];    
    }
    list->data[1 + index] = data;
    list->count++;
}

void list_replace(struct list* list, void* data, const uint64_t index)
{
    list->data[1 + index] = data;
}

void* list_remove(struct list* list, const uint64_t index)
{
    if (index >= list->count)
    {
        fprintf(stderr, "list_remove: index out of bounds\n");
        return NULL;
    }
    void* data_buf = list->data[1 + index];

    for (int i = index + 2; i <= list->count; i++)
    {
        list->data[(i - 1)] = list->data[i];
    }

    list->data[1 + list->count] = NULL;
    list->count--;

    return data_buf;
}

void list_clear(struct list* list)
{
    list->count = 0;
}

void* list_get(struct list* list, const uint64_t index)
{
    if (index >= list->count)
    {
        fprintf(stderr, "list_get: index out of bounds: %lld\n", index);
        return NULL;
    }
    return list->data[1 + index];
}

bool list_contains(struct list* list, const void* data)
{
    for (int i = 0; i < list->count; i++)
    {
        if (list->data[1 + i] == data)
        {
            return true;
        }
    }
    return false;
}


void list_print_int(struct list* list)
{
    printf("(");
    for (uint64_t i = 0; i < list->count; i++)
    {
        printf(i == list->count - 1 ? "%lld" : "%lld, ", (uint64_t)list_get(list, i));
    }
    printf(")");
}

void list_print_char(struct list* list)
{
    printf("(");
    for (uint64_t i = 0; i < list->count; i++)
    {
        printf(i == list->count - 1 ? "%c" : "%c, ", (char)(uint64_t)list_get(list, i));
    }
    printf(")");
}
