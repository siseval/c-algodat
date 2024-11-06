#ifndef hashmap_H
#define hashmap_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define hashmap(K, V) hashmap_create(32, !strcmp(#K, "str") || !strcmp(#K, "char*"))

struct key_value
{
    void* key;
    void* value;
};

struct hashmap
{
    size_t size;
    size_t data_size;
    bool string_hash;
    int count;
    struct key_value** data;
};

struct hashmap* hashmap_create(const size_t size, const bool string_type); void hashmap_destroy(struct hashmap* hashmap_ptr);

void hashmap_put(struct hashmap* hashmap_ptr, void* key_ptr, void* value_ptr);
void* hashmap_remove(struct hashmap* hashmap_ptr, const void* key_ptr);
void hashmap_clear(struct hashmap* hashmap_ptr);

void* hashmap_get(const struct hashmap* hashmap_ptr, const void* key_ptr);
bool hashmap_has_key(const struct hashmap* hashmap_ptr, const void* key_ptr);

#endif
