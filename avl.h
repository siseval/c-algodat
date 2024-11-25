#ifndef AVL_H
#define AVL_H

#include "bst.h"

#define avl(T) avl_create();

struct avl
{
    struct bst* tree;
    struct hashmap* heights;
};

void rotate_left(struct avl* avl, void* vertex);
struct avl* avl_create();
void avl_destroy(struct avl* avl);

void avl_insert(struct avl* avl, void* data);
void avl_remove(struct avl* avl, void* data);

void* avl_get(const struct avl* avl, void* data);
bool avl_contains(const struct avl* avl, void* data);
struct list* avl_get_range(const struct avl* avl, const int64_t from, const int64_t to);

void avl_print_int(const struct avl* avl);

#endif 


