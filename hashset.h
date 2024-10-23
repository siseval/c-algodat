#ifndef HASHSET_H
#define HASHSET_H

#include "types.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define hashset(T) hashset_create(32, T)

struct hashset
{
    size_t size;
    size_t data_size;
    enum type data_type;
    int count;
    void **data;
};

struct hashset* hashset_create(const size_t size, const enum type data_type);
void hashset_destroy(struct hashset* hashset_ptr);

void hashset_put(struct hashset* hashset_ptr, void* data_ptr);
void* hashset_remove(struct hashset* hashset_ptr, const int index);
void hashset_clear(struct hashset* hashset_ptr);

void* hashset_get(const struct hashset* hashset_ptr, const void* data_ptr);
bool hashset_contains(const struct hashset* hashset_ptr, const void* data_ptr);

#endif
