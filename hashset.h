#ifndef hashset_H
#define hashset_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define hashset(T) hashset_create(32, !strcmp(#T, "str") || !strcmp(#T, "char*"))

struct hashset
{
    size_t size;
    size_t data_size;
    bool string_hash;
    int count;
    void **data;
};

struct hashset* hashset_create(const size_t size, const bool string_type);
void hashset_destroy(struct hashset* hashset_ptr);

void hashset_put(struct hashset* hashset_ptr, void* data_ptr);
void* hashset_remove(struct hashset* hashset_ptr, const void* data_ptr);
void hashset_clear(struct hashset* hashset_ptr);

void* hashset_get(const struct hashset* hashset_ptr, const void* data_ptr);
bool hashset_contains(const struct hashset* hashset_ptr, const void* data_ptr);

#endif
