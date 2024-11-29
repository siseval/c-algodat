#include "hashset.h"


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


static bool hashset_realloc(struct hashset* hashset, const uint64_t size)
{
    void** tmp_data = realloc(hashset->data, size);
    bool* tmp_has_data = realloc(hashset->has_data, hashset->size * 2);
    if (!tmp_data || !tmp_has_data)
    {
        fprintf(stderr, "hashset_realloc: realloc failed\n");
        hashset_destroy(hashset);
        return false;
    }
    hashset->data = tmp_data;
    hashset->has_data = tmp_has_data;
    return true;
}

static void hashset_rehash(struct hashset* hashset, const uint64_t size)
{
    void** data_buf = malloc(hashset->data_size * (size + 1)); 
    bool* has_data_buf = malloc(size * 2);
    memcpy(data_buf, hashset->data, hashset->data_size * (size + 1));
    memcpy(has_data_buf, hashset->has_data, hashset->size);
    hashset_clear(hashset);

    for (uint64_t i = 0; i < size; i++)
    {
        if (has_data_buf[i])
        {
            hashset_put(hashset, data_buf[1 + i]);
        }
    }
    free(data_buf);
    free(has_data_buf);
}

static void hashset_fill_hole(struct hashset* hashset, uint64_t index)
{
    uint64_t index_delta = 1;
    while (hashset->has_data[(index + index_delta) % hashset->size])
    {
        void* data = hashset->data[1 + (index + index_delta) % hashset->size];

        uint64_t new_index = hash_data(data, hashset->size, hashset->string_hash);
        if (!(0 < (new_index - index) % hashset->size && (new_index - index) % hashset->size <= index_delta))
        {
            hashset->data[1 + index] = data;
            hashset->has_data[index] = true;
            hashset->data[1 + (index + index_delta) % hashset->size] = NULL;
            hashset->has_data[(index + index_delta) % hashset->size] = false;
            hashset_fill_hole(hashset, (index + index_delta) % hashset->size);
            return;
        }
        index_delta++;
    }
}

struct hashset* hashset_create(const uint64_t size, const bool string_hash)
{
    struct hashset* hashset = calloc(1, sizeof(struct hashset));
    if (!hashset) 
    { 
        fprintf(stderr, "hashset_create: hashset malloc failed\n");
        return NULL; 
    }

    hashset->size = size;
    hashset->data_size = sizeof(void*);
    hashset->string_hash = string_hash;
    hashset->count = 0;

    hashset->has_data = calloc(size, sizeof(bool));
    hashset->data = calloc((1 + size), hashset->data_size);
    if (!hashset->data || !hashset->has_data)
    {
        fprintf(stderr, "hashset_create: data malloc failed\n");
        free(hashset);
        return NULL;
    }

    return hashset;
}

void hashset_destroy(struct hashset* hashset)
{
    free(hashset->has_data);
    free(hashset->data);
    free(hashset);
}

void hashset_put(struct hashset* hashset, void* data)
{
    if (hashset->count >= hashset->size / 2)
    {
        if (!hashset_realloc(hashset, hashset->data_size + (hashset->size * 2) * hashset->data_size))
        {
            return;
        }
        hashset->size *= 2;
        hashset_rehash(hashset, hashset->size / 2);
    }

    uint64_t index = hash_data(data, hashset->size, hashset->string_hash);

    while (hashset->has_data[index])
    {
        if (hashset->data[1 + index] == data)
        {
            return;
        }
        index = (index + 1) % hashset->size;
    }
    hashset->data[1 + index] = data;
    hashset->has_data[index] = true;
    hashset->count++;
}

void* hashset_remove(struct hashset* hashset, const void* data)
{  
    uint64_t index = hash_data(data, hashset->size, hashset->string_hash);
    while (hashset->data[1 + index] != data)
    {
        if (hashset->has_data[1 + index])
        {
            fprintf(stderr, "hashset_remove: value not in hashset.\n");
            return NULL;
        }
        index = (index + 1) % hashset->size;
    }
    void* data_buf = hashset->data[1 + index];
    hashset->data[1 + index] = NULL;
    hashset->has_data[index] = false;
    hashset->count--;
    hashset_fill_hole(hashset, index);
    return data_buf;
}

void hashset_clear(struct hashset* hashset)
{
    for (uint64_t i = 0; i < hashset->size; i++)
    {
        hashset->data[1 + i] = NULL;
        hashset->has_data[i] = false;
    }
    hashset->count = 0;
}

void* hashset_get(const struct hashset* hashset, const void* data)
{
    uint64_t index = hash_data(data, hashset->size, hashset->string_hash);
    while (hashset->data[1 + index] != data)
    {
        if (hashset->has_data[1 + index])
        {
            fprintf(stderr, "hashset_get: value not in hashset.\n");
            return NULL;
        }
        index = (index + 1) % hashset->size;
    }
    return hashset->data[1 + index];
}

void* hashset_get_random(const struct hashset* hashset)
{
    srand(time(NULL));
    if (hashset->count <= 0)
    {
        fprintf(stderr, "hashset_get_random: hashset is empty.\n");
    }
    uint64_t index = rand() % hashset->size;
    while (hashset->has_data[1 + index])
    {
        index = (index + 1) % hashset->size;
    }
    return hashset->data[1 + index];
}

bool hashset_contains(const struct hashset* hashset, const void* data)
{
    uint64_t index = hash_data(data, hashset->size, hashset->string_hash);
    while (hashset->data[1 + index] != data)
    {
        if (hashset->has_data[1 + index])
        {
            return false;
        }
        index = (index + 1) % hashset->size;
    }
    return true;
}
