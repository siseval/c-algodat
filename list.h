#ifndef LIST_H
#define LIST_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>

#define list(T) list_create(32)

struct list
{
    uint64_t size;
    uint64_t data_size;
    uint64_t count;
    void **data;
};

struct list* list_create(const uint64_t size);
struct list* list_create_copy(const struct list* list);
void list_destroy(struct list* list);

void list_append(struct list* list, void* data);
void list_insert(struct list* list, void* data, const uint64_t index);
void list_replace(struct list* list, void* data, const uint64_t index);

void* list_remove_at(struct list* list, const uint64_t index);
void* list_remove(struct list* list, void* data);
void list_clear(struct list* list);

struct list* list_reverse(struct list* list);
struct list* list_swap(struct list* list, const uint64_t index_a, const uint64_t index_b);

struct list* list_get_sublist(const struct list* list, const uint64_t from_index, const uint64_t to_index);

void* list_get(const struct list* list, const uint64_t index);
bool list_contains(const struct list* list, const void* data);

bool list_is_sorted(struct list* list);

struct list* list_bubble_sort(struct list* list);
struct list* list_selection_sort(struct list* list);
struct list* list_insertion_sort(struct list* list);
struct list* list_heap_sort(struct list* list);
struct list* list_merge_sort(struct list* list);
struct list* list_quick_sort(struct list* list);

void list_print_int(const struct list* list);
void list_print_char(const struct list* list);

#endif
