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
    uint64_t size;
    uint64_t data_size;
    bool string_hash;
    int count;
    void **data;
};

struct hashset* hashset_create(const uint64_t size, const bool string_type);
void hashset_destroy(struct hashset* hashset);

void hashset_put(struct hashset* hashset, void* data);
void* hashset_remove(struct hashset* hashset, const void* data);
void hashset_clear(struct hashset* hashset);

void* hashset_get(const struct hashset* hashset, const void* data);
void* hashset_get_random(const struct hashset* hashset);
bool hashset_contains(const struct hashset* hashset, const void* data);

#endif
