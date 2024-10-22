#ifndef LIST_H
#define LIST_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define list(T) list_create(32, sizeof(T*));

struct list
{
    size_t size;
    size_t data_size;
    int count;
    void **data;
};

struct list* list_create(size_t size, size_t data_size);
void list_destroy(struct list* list_ptr);

void list_append(struct list* list_ptr, void* data_ptr);
void* list_remove(struct list* list_ptr, int index);
void list_clear(struct list* list_ptr);

void* list_get(struct list* list_ptr, int index);

#endif
