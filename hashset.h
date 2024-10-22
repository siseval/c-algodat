#include <stdlib.h>
#include <stdio.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define hashset(T) hashset_create(32, sizeof(T*));

struct hashset
{
    size_t size;
    size_t data_size;
    int count;
    void **data;
};

struct hashset* hashset_create(size_t size, size_t data_size);
void hashset_destroy(struct hashset* hashset_ptr);

void hashset_put(struct hashset* hashset_ptr, void* data_ptr);
void* hashset_remove(struct hashset* hashset_ptr, int index);
void hashset_clear(struct hashset* hashset_ptr);

void* hashset_get(struct hashset* hashset_ptr, void* data_ptr);
bool hashset_contains(struct hashset* hashset_ptr, void* data_ptr);
