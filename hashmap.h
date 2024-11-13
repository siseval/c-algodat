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
    uint64_t size;
    uint64_t data_size;
    bool string_hash;
    int count;
    struct key_value** data;
};

struct hashmap* hashmap_create(const uint64_t size, const bool string_type); void hashmap_destroy(struct hashmap* hashmap);

void hashmap_put(struct hashmap* hashmap, void* key, void* value);
void* hashmap_remove(struct hashmap* hashmap, const void* key);
void hashmap_clear(struct hashmap* hashmap);

void* hashmap_get(const struct hashmap* hashmap, const void* key);
bool hashmap_has_key(const struct hashmap* hashmap, const void* key);

#endif
