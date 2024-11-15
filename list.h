#ifndef LIST_H
#define LIST_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define list(T) list_create(32)

struct list
{
    uint64_t size;
    uint64_t data_size;
    uint64_t count;
    void **data;
};

struct list* list_create(const uint64_t size);
void list_destroy(struct list* list);

void list_append(struct list* list, void* data);
void list_insert(struct list* list, void* data, const uint64_t index);
void list_replace(struct list* list, void* data, const uint64_t index);

void* list_remove(struct list* list, const uint64_t index);
void list_clear(struct list* list);

struct list* list_reverse(struct list* list);

void* list_get(struct list* list, const uint64_t index);
bool list_contains(struct list* list, const void* data);

void list_print_int(struct list* list);
void list_print_char(struct list* list);

#endif
