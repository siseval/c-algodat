#ifndef AVL_H
#define AVL_H

#include "bst.h"

#define avl(T) avl_create();

struct avl
{
    struct bst* tree;
    struct hashmap* heights;
};

struct avl* avl_create();
void avl_destroy(struct avl* avl);

void avl_insert(struct avl* avl, int64_t data);
void avl_remove(struct avl* avl, int64_t data);

void* avl_get(const struct avl* avl, int64_t data);
bool avl_contains(const struct avl* avl, int64_t data);
struct list* avl_get_range(const struct avl* avl, const int64_t from, const int64_t to);

void avl_print_int(const struct avl* avl);

#endif 


