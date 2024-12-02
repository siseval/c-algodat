#include "hashmap.h"


static uint64_t hash_integer(const void* data, const uint64_t mod)
{
    return (3197 * (uint64_t)data) % mod;
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

static bool hashmap_realloc(struct hashmap* hashmap, const uint64_t size)
{
    void** tmp_keys = realloc(hashmap->keys, size);
    void** tmp_values = realloc(hashmap->values, size);
    bool* tmp_has_data = realloc(hashmap->has_data, hashmap->size * 2);
    if (!tmp_keys || !tmp_values || !tmp_has_data)
    {
        fprintf(stderr, "hashmap_realloc: realloc failed\n");
        hashmap_destroy(hashmap);
        return false;
    }
    hashmap->keys = tmp_keys;
    hashmap->values = tmp_values;
    hashmap->has_data = tmp_has_data;
    return true;
}

static void hashmap_rehash(struct hashmap* hashmap, const uint64_t size)
{
    void** keys_buf = malloc(hashmap->data_size * (size + 1)); 
    void** values_buf = malloc(hashmap->data_size * (size + 1)); 
    bool* has_data_buf = malloc(size * 2);
    memcpy(keys_buf, hashmap->keys, hashmap->data_size * (size + 1));
    memcpy(values_buf, hashmap->values, hashmap->data_size * (size + 1));
    memcpy(has_data_buf, hashmap->has_data, hashmap->size);
    hashmap_clear(hashmap);

    for (uint64_t i = 0; i < size; i++)
    {
        if (has_data_buf[i])
        {
            hashmap_put(hashmap, keys_buf[1 + i], values_buf[1 + i]);
        }
    }
    free(keys_buf);
    free(values_buf);
    free(has_data_buf);
}

static void hashmap_fill_hole(struct hashmap* hashmap, const uint64_t index)
{
    uint64_t index_delta = 1; 
    while (hashmap->has_data[(index + index_delta) % hashmap->size])
    {
        void* key = hashmap->keys[1 + (index + index_delta) % hashmap->size];
        void* value = hashmap->values[1 + (index + index_delta) % hashmap->size];

        uint64_t new_index = hash_data(key, hashmap->size, hashmap->string_hash);
        if (!(0 < (new_index - index) % hashmap->size && (new_index - index) % hashmap->size <= index_delta))
        {
            hashmap->keys[1 + index] = key;
            hashmap->values[1 + index] = value;
            hashmap->has_data[index] = true;
            hashmap->keys[1 + (index + index_delta) % hashmap->size] = NULL;
            hashmap->values[1 + (index + index_delta) % hashmap->size] = NULL;
            hashmap->has_data[(index + index_delta) % hashmap->size] = false;
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
    hashmap->data_size = sizeof(void*);
    hashmap->string_hash = string_hash;
    hashmap->count = 0;

    hashmap->has_data = calloc(size, sizeof(bool));
    hashmap->keys = calloc((1 + size), hashmap->data_size);
    hashmap->values = calloc((1 + size), hashmap->data_size);
    if (!hashmap->has_data || !hashmap->keys || !hashmap->values)
    {
        fprintf(stderr, "hashmap_create: data malloc failed\n");
        free(hashmap);
        return NULL;
    }

    return hashmap;
}

void hashmap_destroy(struct hashmap* hashmap)
{
    free(hashmap->has_data);
    free(hashmap->keys);
    free(hashmap->values);
    free(hashmap);
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
        hashmap_rehash(hashmap, hashmap->size / 2);
    }

    uint64_t index = hash_data(key, hashmap->size, hashmap->string_hash);

    while (hashmap->has_data[index])
    {
        if (key_equals(hashmap->keys[1 + index], key, hashmap->string_hash))
        {
            hashmap->values[1 + index] = value;
            return;
        }
        index = (index + 1) % hashmap->size;
    }

    hashmap->keys[1 + index] = key;
    hashmap->values[1 + index] = value;
    hashmap->has_data[index] = true;
    hashmap->count++;
}

void* hashmap_remove(struct hashmap* hashmap, const void* key)
{
    uint64_t index = hash_data(key, hashmap->size, hashmap->string_hash);    
    while (hashmap->has_data[index])
    {
        if (key_equals(hashmap->keys[1 + index], key, hashmap->string_hash))
        {
            void* value = hashmap->values[1 + index];
            hashmap->keys[1 + index] = NULL;
            hashmap->values[1 + index] = NULL;
            hashmap->has_data[index] = false;
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
    for (uint64_t i = 0; i < hashmap->size; i++)
    {
        hashmap->keys[1 + i] = NULL;
        hashmap->values[1 + i] = NULL;
        hashmap->has_data[i] = false;
    }
    hashmap->count = 0;
}

void* hashmap_get(const struct hashmap* hashmap, const void* key)
{
    uint64_t index = hash_data(key, hashmap->size, hashmap->string_hash);
    while (hashmap->has_data[index])
    {
        if (key_equals(hashmap->keys[1 + index], key, hashmap->string_hash))
        {
            return hashmap->values[1 + index];
        }
        index = (index + 1) % hashmap->size;
    }
    fprintf(stderr, "hashmap_get: value not in hashmap.\n");
    return NULL;
}

bool hashmap_has_key(const struct hashmap* hashmap, const void* key)
{
    uint64_t index = hash_data(key, hashmap->size, hashmap->string_hash);
    while (hashmap->has_data[index])
    {
        if (key_equals(hashmap->keys[1 + index], key, hashmap->string_hash))
        {
            return true;
        }
        index = (index + 1) % hashmap->size;
    }
    return false;
}
