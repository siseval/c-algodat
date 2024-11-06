#include "hashmap.h"


static uint32_t hash_integer(const void* data_ptr, const uint32_t mod)
{
    uint32_t hash = (((uint32_t)(mod * 0.00003) * ((uint64_t)data_ptr)));
    return hash % mod;
}

static uint32_t hash_string(const char* data_ptr, const uint32_t mod)
{
    uint32_t hash = 7;
    for (int i = 0; i < strlen(data_ptr); i++)
    {
        hash = 31 * hash + data_ptr[i];
    }
    return hash % mod;
}

static uint32_t hash_data(const void* data_ptr, const uint32_t mod, bool string_hash)
{
    return string_hash ? hash_string(data_ptr, mod) : hash_integer(data_ptr, mod); 
}

static bool key_equals(const void* key1_ptr, const void* key2_ptr, bool string_hash)
{
    if (string_hash)
    {
        return !strcmp((char*)key1_ptr, (char*)key2_ptr);
    }
    return key1_ptr == key2_ptr;
}

static bool hashmap_realloc(struct hashmap* hashmap_ptr, size_t size)
{
    struct key_value** tmp = realloc(hashmap_ptr->data, size);
    if (!tmp)
    {
        fprintf(stderr, "hashmap_realloc: realloc failed\n");
        hashmap_destroy(hashmap_ptr);
        return false;
    }
    hashmap_ptr->data = tmp;
    return true;
}

static void hashmap_rehash(struct hashmap* hashmap_ptr)
{
    struct key_value** data_buf = calloc(hashmap_ptr->size, hashmap_ptr->data_size); 
    memcpy(data_buf, hashmap_ptr->data, hashmap_ptr->size * hashmap_ptr->data_size);
    hashmap_clear(hashmap_ptr);
    for (int i = 1; i < hashmap_ptr->size; i++)
    {
        if (data_buf[i] != NULL)
        {
            hashmap_put(hashmap_ptr, data_buf[i]->key, data_buf[i]->value);
        }
    }
    free(data_buf);
}

static void hashmap_fill_hole(struct hashmap* hashmap_ptr, const uint32_t index)
{
    uint32_t index_delta = 1; 
    while (hashmap_ptr->data[(index + index_delta) % hashmap_ptr->size] != NULL)
    {
        struct key_value* data_ptr = hashmap_ptr->data[(index + index_delta) % hashmap_ptr->size];
        uint32_t new_index = hash_data(data_ptr->key, hashmap_ptr->size, hashmap_ptr->string_hash);
        if (!(0 < (new_index - index) % hashmap_ptr->size && (new_index - index) % hashmap_ptr->size <= index_delta))
        {
            hashmap_ptr->data[index] = data_ptr;
            hashmap_ptr->data[index + index_delta % hashmap_ptr->size] = NULL;
            hashmap_fill_hole(hashmap_ptr, new_index);
            return;
        }
        index_delta++;
    }
}

struct hashmap* hashmap_create(const size_t size, const bool string_hash)
{
    struct hashmap* hashmap_ptr = calloc(1, sizeof(struct hashmap));
    if (!hashmap_ptr) 
    { 
        fprintf(stderr, "hashmap_create: hashmap malloc failed\n");
        return NULL; 
    }

    hashmap_ptr->size = size;
    hashmap_ptr->data_size = sizeof(struct key_value*);
    hashmap_ptr->string_hash = string_hash;
    hashmap_ptr->count = 0;

    hashmap_ptr->data = calloc((1 + size), hashmap_ptr->data_size);
    if (!hashmap_ptr->data) 
    {
        fprintf(stderr, "hashmap_create: data malloc failed\n");
        free(hashmap_ptr);
        return NULL;
    }

    return hashmap_ptr;
}

void hashmap_destroy(struct hashmap* hashmap_ptr)
{
    free(hashmap_ptr->data);
    free(hashmap_ptr);
    for (int i = 1; i <= hashmap_ptr->count; i++)
    {
        free(hashmap_ptr->data[i]->key);
        free(hashmap_ptr->data[i]->value);
        free(hashmap_ptr->data[i]);
    }
}

void hashmap_put(struct hashmap* hashmap_ptr, void* key_ptr, void* value_ptr)
{
    if (hashmap_ptr->count >= hashmap_ptr->size / 2)
    {
        if (!hashmap_realloc(hashmap_ptr, hashmap_ptr->data_size + (hashmap_ptr->size * 2) * hashmap_ptr->data_size))
        {
            return;
        }
        hashmap_ptr->size *= 2;
        hashmap_rehash(hashmap_ptr);
    }

    uint32_t index = hash_data(key_ptr, hashmap_ptr->size, hashmap_ptr->string_hash);

    struct key_value* key_value_ptr = calloc(1, sizeof(struct key_value));
    key_value_ptr->key = key_ptr;
    key_value_ptr->value = value_ptr;

    while (hashmap_ptr->data[1 + index] != NULL)
    {
        if (key_equals(hashmap_ptr->data[1 + index]->key, key_ptr, hashmap_ptr->string_hash))
        {
            return;
        }
        index = (index + 1) % hashmap_ptr->size;
    }
    hashmap_ptr->data[1 + index] = key_value_ptr;
    hashmap_ptr->count++;
}

void* hashmap_remove(struct hashmap* hashmap_ptr, const void* key_ptr)
{
    uint32_t index = hash_data(key_ptr, hashmap_ptr->size, hashmap_ptr->string_hash);    
    while (hashmap_ptr->data[1 + index] != NULL)
    {
        if (key_equals(hashmap_ptr->data[1 + index]->key, key_ptr, hashmap_ptr->string_hash))
        {
            void* value = hashmap_ptr->data[1 + index]->value;
            free(hashmap_ptr->data[1 + index]);
            hashmap_ptr->data[1 + index] = NULL;
            hashmap_ptr->count--;
            hashmap_fill_hole(hashmap_ptr, 1 + index);
            return value;
        }
        index = (index + 1) % hashmap_ptr->size;
    }
    fprintf(stderr, "hashmap_remove: value not in hashmap.\n");
    return NULL;
}

void hashmap_clear(struct hashmap* hashmap_ptr)
{
    for (int i = 0; i < hashmap_ptr->size; i++)
    {
        hashmap_ptr->data[i] = NULL;
    }
    hashmap_ptr->count = 0;
}

void* hashmap_get(const struct hashmap* hashmap_ptr, const void* key_ptr)
{
    uint32_t index = hash_data(key_ptr, hashmap_ptr->size, hashmap_ptr->string_hash);
    while (hashmap_ptr->data[1 + index] != NULL)
    {
        if (key_equals(hashmap_ptr->data[1 + index]->key, key_ptr, hashmap_ptr->string_hash))
        {
            return hashmap_ptr->data[1 + index]->value;
        }
        index = (index + 1) % hashmap_ptr->size;
    }
    fprintf(stderr, "hashmap_get: value not in hashmap.\n");
    return NULL;
}

bool hashmap_has_key(const struct hashmap* hashmap_ptr, const void* key_ptr)
{
    uint32_t index = hash_data(key_ptr, hashmap_ptr->size, hashmap_ptr->string_hash);
    while (hashmap_ptr->data[1 + index] != NULL)
    {
        if (key_equals(hashmap_ptr->data[1 + index]->key, key_ptr, hashmap_ptr->string_hash))
        {
            return true;
        }
        index = (index + 1) % hashmap_ptr->size;
    }
    return false;
}
