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


static bool hashset_realloc(struct hashset* hashset, uint64_t size)
{
    void** tmp = realloc(hashset->data, size);
    if (!tmp)
    {
        fprintf(stderr, "hashset_realloc: realloc failed\n");
        free(hashset->data);
        free(hashset);
        return false;
    }
    hashset->data = tmp;
    return true;
}

static void hashset_rehash(struct hashset* hashset)
{
    void** data_buf = calloc(hashset->size, hashset->data_size); 
    memcpy(data_buf, hashset->data, hashset->size * hashset->data_size);
    hashset_clear(hashset);
    for (int i = 0; i < hashset->size; i++)
    {
        if (data_buf[i] != NULL)
        {
            hashset_put(hashset, data_buf[i]);
        }
    }
    free(data_buf);
}

static void hashset_fill_hole(struct hashset* hashset, uint64_t index)
{
    uint64_t index_delta = 1;
    while (hashset->data[1 + (index + index_delta) % hashset->size] != NULL)
    {
        void* data = hashset->data[1 + (index + index_delta) % hashset->size];
        uint64_t new_index = hash_data(data, hashset->size, hashset->string_hash);
        if (!(0 < (new_index - index) % hashset->size && (new_index - index) % hashset->size <= index_delta))
        {
            hashset->data[1 + index] = data;
            hashset->data[1 + (index + index_delta) % hashset->size] = NULL;
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

    hashset->data = calloc((1 + size), hashset->data_size);
    if (!hashset->data) 
    {
        fprintf(stderr, "hashset_create: data malloc failed\n");
        free(hashset);
        return NULL;
    }

    return hashset;
}

void hashset_destroy(struct hashset* hashset)
{
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
        hashset_rehash(hashset);
    }

    uint64_t index = hash_data(data, hashset->size, hashset->string_hash);

    while (hashset->data[1 + index] != NULL)
    {
        if (hashset->data[1 + index] == data)
        {
            return;
        }
        index = (index + 1) % hashset->size;
    }
    hashset->data[1 + index] = data;
    hashset->count++;
}

void* hashset_remove(struct hashset* hashset, const void* data)
{  
    uint64_t index = hash_data(data, hashset->size, hashset->string_hash);
    while (hashset->data[1 + index] != data)
    {
        if (hashset->data[1 + index] == NULL)
        {
            fprintf(stderr, "hashset_remove: value not in hashset.\n");
            return NULL;
        }
        index = (index + 1) % hashset->size;
    }
    void* data_buf = hashset->data[1 + index];
    hashset->data[1 + index] = NULL;
    hashset->count--;
    hashset_fill_hole(hashset, index);
    return data_buf;
}

void hashset_clear(struct hashset* hashset)
{
    for (uint64_t i = 0; i < hashset->size; i++)
    {
        hashset->data[i] = NULL;
    }
    hashset->count = 0;
}

void* hashset_get(const struct hashset* hashset, const void* data)
{
    uint64_t index = hash_data(data, hashset->size, hashset->string_hash);
    while (hashset->data[1 + index] != data)
    {
        if (hashset->data[1 + index] == NULL)
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
    while (hashset->data[1 + index] == NULL)
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
        if (hashset->data[1 + index] == NULL)
        {
            return false;
        }
        index = (index + 1) % hashset->size;
    }
    return true;
}
