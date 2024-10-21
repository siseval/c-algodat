#include "list.h"


struct list* list_create(size_t size, size_t data_size)
{
    struct list* list_ptr = malloc(sizeof(struct list));
    if (!list_ptr) { return NULL; }

    list_ptr->size = size;
    list_ptr->count = 0;
    list_ptr->data_size = data_size;
    list_ptr->data = malloc((size + 1) * data_size);

    if (!list_ptr->data) 
    {
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

void list_append(struct list* list_ptr, void* data)
{
    if (list_ptr->count >= list_ptr->size)
    {
        void* data_buf = realloc(*list_ptr->data, + list_ptr->size * 2 * list_ptr->data_size);
        if (!data_buf)
        {
            fprintf(stderr, "list_append: realloc failed\n");
            free(list_ptr->data);
            free(list_ptr);
            return;
        }
        *list_ptr->data = data_buf;
        list_ptr->size *= 2;
    }
    list_ptr->data[(list_ptr->count + 1) * list_ptr->data_size] = data;
    list_ptr->count++;
}

void* list_remove(struct list* list_ptr, int index)
{
    void* data_buf = list_ptr->data + index * list_ptr->data_size;
    for (int i = index + 1; i < list_ptr->count - 1; i++)
    {
        list_ptr->data[(i - 1) * list_ptr->data_size] = list_ptr->data[i * list_ptr->data_size];
    }
    list_ptr->count--;
    list_ptr->data[list_ptr->count * list_ptr->data_size] = NULL;
    return data_buf;
}

void list_clear(struct list* list_ptr)
{
    for (int i = 0; i < list_ptr->count; i++)
    {
        list_remove(list_ptr, 0);
    }
}

void* list_get(struct list* list_ptr, int index)
{
    return list_ptr->data + (index + 1) * list_ptr->data_size;
}

