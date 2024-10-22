#include "hashset.h"


uint32_t hash_data(void* data_ptr, uint32_t mod)
{
    uint32_t hash = (((uint32_t)(mod * 0.00003) * ((long)data_ptr)) % mod);
    return hash;
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

struct hashset* hashset_create(size_t size, size_t data_size)
{
    struct hashset* hashset_ptr = malloc(sizeof(struct hashset));
    if (!hashset_ptr) 
    { 
        fprintf(stderr, "hashset_create: hashset malloc failed\n");
        return NULL; 
    }

    hashset_ptr->size = size;
    hashset_ptr->data_size = data_size;
    hashset_ptr->count = 0;
    hashset_ptr->data = malloc((1 + size) * data_size);

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
        void** tmp = realloc(hashset_ptr->data, hashset_ptr->data_size + (hashset_ptr->size * 2) * hashset_ptr->data_size);
        if (!tmp)
        {
            fprintf(stderr, "hashset_append: realloc failed\n");
            free(hashset_ptr->data);
            free(hashset_ptr);
            return;
        }
        hashset_ptr->data = tmp;
        hashset_ptr->size *= 2;
        hashset_rehash(hashset_ptr);
    }

    uint32_t index = hash_data(data_ptr, hashset_ptr->size);
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

void* hashset_remove(struct hashset* hashset_ptr, int index)
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

void* hashset_get(struct hashset* hashset_ptr, void* data_ptr)
{
    uint32_t index = hash_data(data_ptr, hashset_ptr->size);
    while (hash_data(hashset_ptr->data[1 + index], hashset_ptr->size) != hash_data(data_ptr, hashset_ptr->size))
    {
        if (hashset_ptr->data[index] == NULL)
        {
            fprintf(stderr, "hashset_get: value not in hashset.\n");
            return NULL;
        }
        index = (index + 1) % hashset_ptr->size;
    }
    return hashset_ptr->data[1 + index];
}

