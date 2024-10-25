#include "hashset.h"


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

bool hashset_realloc(struct hashset* hashset_ptr, size_t size)
{
    void** tmp = realloc(hashset_ptr->data, size);
    if (!tmp)
    {
        fprintf(stderr, "hashset_realloc: realloc failed\n");
        free(hashset_ptr->data);
        free(hashset_ptr);
        return false;
    }
    hashset_ptr->data = tmp;
    return true;
}

void hashset_rehash(struct hashset* hashset_ptr)
{
    void** data_buf = malloc(hashset_ptr->size * hashset_ptr->data_size); 
    memcpy(data_buf, hashset_ptr->data, hashset_ptr->size * hashset_ptr->data_size);
    hashset_clear(hashset_ptr);
    for (int i = 0; i < hashset_ptr->size; i++)
    {
        if (data_buf[i] != NULL)         
        {
            hashset_put(hashset_ptr, data_buf[i]);
        }
    }
    free(data_buf);
}

struct hashset* hashset_create(const size_t size, const bool string_hash)
{
    struct hashset* hashset_ptr = malloc(sizeof(struct hashset));
    if (!hashset_ptr) 
    { 
        fprintf(stderr, "hashset_create: hashset malloc failed\n");
        return NULL; 
    }

    hashset_ptr->size = size;
    hashset_ptr->data_size = sizeof(void*);
    hashset_ptr->string_hash = string_hash;
    hashset_ptr->count = 0;

    hashset_ptr->data = malloc((1 + size) * hashset_ptr->data_size);
    if (!hashset_ptr->data) 
    {
        fprintf(stderr, "hashset_create: data malloc failed\n");
        free(hashset_ptr);
        return NULL;
    }

    return hashset_ptr;
}

void hashset_destroy(struct hashset* hashset_ptr)
{
    free(hashset_ptr->data);
    free(hashset_ptr);
}

void hashset_put(struct hashset* hashset_ptr, void* data_ptr)
{
    if (hashset_ptr->count >= hashset_ptr->size / 2)
    {
        if (!hashset_realloc(hashset_ptr, hashset_ptr->data_size + (hashset_ptr->size * 2) * hashset_ptr->data_size))
        {
            return;
        }
        hashset_ptr->size *= 2;
        hashset_rehash(hashset_ptr);
    }

    uint32_t index = hash_data(data_ptr, hashset_ptr->size, hashset_ptr->string_hash);

    while (hashset_ptr->data[1 + index] != NULL)
    {
        if (hashset_ptr->data[1 + index] == data_ptr)
        {
            return;
        }
        index = (index + 1) % hashset_ptr->size;
    }
    hashset_ptr->data[1 + index] = data_ptr;
    hashset_ptr->count++;
}

void* hashset_remove(struct hashset* hashset_ptr, const int index)
{
    if (index >= hashset_ptr->count)
    {
        fprintf(stderr, "hashset_remove: index out of bounds\n");
        return NULL;
    }
    void* data_buf = hashset_ptr->data[index];

    for (int i = index + 2; i <= hashset_ptr->count; i++)
    {
        hashset_ptr->data[(i - 1)] = hashset_ptr->data[i];
    }

    hashset_ptr->data[1 + hashset_ptr->count] = NULL;
    hashset_ptr->count--;

    return data_buf;
}

void hashset_clear(struct hashset* hashset_ptr)
{
    for (int i = 0; i < hashset_ptr->size; i++)
    {
        hashset_ptr->data[i] = NULL;
    }
    hashset_ptr->count = 0;
}

void* hashset_get(const struct hashset* hashset_ptr, const void* data_ptr)
{
    uint32_t index = hash_data(data_ptr, hashset_ptr->size, hashset_ptr->string_hash);
    while (hashset_ptr->data[1 + index] != data_ptr)
    {
        if (hashset_ptr->data[1 + index] == NULL)
        {
            fprintf(stderr, "hashset_get: value not in hashset.\n");
            return NULL;
        }
        index = (index + 1) % hashset_ptr->size;
    }
    return hashset_ptr->data[1 + index];
}

bool hashset_contains(const struct hashset* hashset_ptr, const void* data_ptr)
{
    uint32_t index = hash_data(data_ptr, hashset_ptr->size, hashset_ptr->string_hash);
    while (hashset_ptr->data[1 + index] != data_ptr)
    {
        if (hashset_ptr->data[1 + index] == NULL)
        {
            return false;
        }
        index = (index + 1) % hashset_ptr->size;
    }
    return true;
}
