#include "hashmap.h"


static uint64_t hash_integer(const void* data, const uint64_t mod)
{
    uint64_t hash = (3197 * (uint64_t)data);
    return hash % mod;
}

static uint64_t hash_string(const char* data, const uint64_t mod)
{
    uint64_t hash = 7;
    for (int i = 0; i < strlen(data); i++)
    {
        hash = 31 * hash + data[i];
    }
    return hash % mod;
}

static uint64_t hash_data(const void* data, const uint64_t mod, bool string_hash)
{
    return string_hash ? hash_string(data, mod) : hash_integer(data, mod); 
}

static bool key_equals(const void* key1, const void* key2, bool string_hash)
{
    if (string_hash)
    {
        return !strcmp((char*)key1, (char*)key2);
    }
    return key1 == key2;
}

static bool hashmap_realloc(struct hashmap* hashmap, uint64_t size)
{
    struct key_value** tmp = realloc(hashmap->data, size);
    if (!tmp)
    {
        fprintf(stderr, "hashmap_realloc: realloc failed\n");
        hashmap_destroy(hashmap);
        return false;
    }
    hashmap->data = tmp;
    return true;
}

static void hashmap_rehash(struct hashmap* hashmap)
{
    struct key_value** data_buf = calloc(hashmap->size, hashmap->data_size); 
    memcpy(data_buf, hashmap->data, hashmap->size * hashmap->data_size);
    hashmap_clear(hashmap);
    for (int i = 1; i < hashmap->size; i++)
    {
        if (data_buf[i] != NULL)
        {
            hashmap_put(hashmap, data_buf[i]->key, data_buf[i]->value);
        }
    }
    free(data_buf);
}

static void hashmap_fill_hole(struct hashmap* hashmap, const uint64_t index)
{
    uint64_t index_delta = 1; 
    while (hashmap->data[1 + (index + index_delta) % hashmap->size] != NULL)
    {
        struct key_value* data = hashmap->data[1 + (index + index_delta) % hashmap->size];
        uint64_t new_index = hash_data(data->key, hashmap->size, hashmap->string_hash);
        if (!(0 < (new_index - index) % hashmap->size && (new_index - index) % hashmap->size <= index_delta))
        {
            hashmap->data[1 + index] = data;
            hashmap->data[1 + (index + index_delta) % hashmap->size] = NULL;
            hashmap_fill_hole(hashmap, (index + index_delta) % hashmap->size);
            return;
        }
        index_delta++;
    }
}

struct hashmap* hashmap_create(const uint64_t size, const bool string_hash)
{
    struct hashmap* hashmap = calloc(1, sizeof(struct hashmap));
    if (!hashmap) 
    { 
        fprintf(stderr, "hashmap_create: hashmap malloc failed\n");
        return NULL; 
    }

    hashmap->size = size;
    hashmap->data_size = sizeof(struct key_value*);
    hashmap->string_hash = string_hash;
    hashmap->count = 0;

    hashmap->data = calloc((1 + size), hashmap->data_size);
    if (!hashmap->data) 
    {
        fprintf(stderr, "hashmap_create: data malloc failed\n");
        free(hashmap);
        return NULL;
    }

    return hashmap;
}

void hashmap_destroy(struct hashmap* hashmap)
{
    free(hashmap->data);
    free(hashmap);
    for (int i = 1; i <= hashmap->count; i++)
    {
        free(hashmap->data[i]->key);
        free(hashmap->data[i]->value);
        free(hashmap->data[i]);
    }
}

void hashmap_put(struct hashmap* hashmap, void* key, void* value)
{
    if (hashmap->count >= hashmap->size / 2)
    {
        if (!hashmap_realloc(hashmap, hashmap->data_size + (hashmap->size * 2) * hashmap->data_size))
        {
            return;
        }
        hashmap->size *= 2;
        hashmap_rehash(hashmap);
    }

    uint64_t index = hash_data(key, hashmap->size, hashmap->string_hash);

    while (hashmap->data[1 + index] != NULL)
    {
        if (key_equals(hashmap->data[1 + index]->key, key, hashmap->string_hash))
        {
            hashmap->data[1 + index]->value = value;
            return;
        }
        index = (index + 1) % hashmap->size;
    }

    struct key_value* key_value = calloc(1, sizeof(struct key_value));
    key_value->key = key;
    key_value->value = value;

    hashmap->data[1 + index] = key_value;
    hashmap->count++;
}

void* hashmap_remove(struct hashmap* hashmap, const void* key)
{
    uint64_t index = hash_data(key, hashmap->size, hashmap->string_hash);    
    while (hashmap->data[1 + index] != NULL)
    {
        if (key_equals(hashmap->data[1 + index]->key, key, hashmap->string_hash))
        {
            void* value = hashmap->data[1 + index]->value;
            free(hashmap->data[1 + index]);
            hashmap->data[1 + index] = NULL;
            hashmap->count--;
            hashmap_fill_hole(hashmap, index);
            return value;
        }
        index = (index + 1) % hashmap->size;
    }
    fprintf(stderr, "hashmap_remove: value not in hashmap.\n");
    return NULL;
}

void hashmap_clear(struct hashmap* hashmap)
{
    for (int i = 0; i < hashmap->size; i++)
    {
        hashmap->data[i] = NULL;
    }
    hashmap->count = 0;
}

void* hashmap_get(const struct hashmap* hashmap, const void* key)
{
    uint64_t index = hash_data(key, hashmap->size, hashmap->string_hash);
    while (hashmap->data[1 + index] != NULL)
    {
        if (key_equals(hashmap->data[1 + index]->key, key, hashmap->string_hash))
        {
            return hashmap->data[1 + index]->value;
        }
        index = (index + 1) % hashmap->size;
    }
    fprintf(stderr, "hashmap_get: value not in hashmap.\n");
    return NULL;
}

bool hashmap_has_key(const struct hashmap* hashmap, const void* key)
{
    uint64_t index = hash_data(key, hashmap->size, hashmap->string_hash);
    while (hashmap->data[1 + index] != NULL)
    {
        if (key_equals(hashmap->data[1 + index]->key, key, hashmap->string_hash))
        {
            return true;
        }
        index = (index + 1) % hashmap->size;
    }
    return false;
}
