#ifndef AVL_H
#define AVL_H

#include "bst.h"

struct avl
{
    struct bst* tree;
};

struct avl* avl_create();
void avl_destroy(struct avl* avl);

void avl_insert(struct avl* avl);
void avl_remove(struct avl* avl);

void* avl_get(const struct avl*, void* vertex);
void* avl_contains(const struct avl*, void* vertex);

void* avl_get_parent(const struct avl* avl, void* vertex);
void* avl_get_left(const struct avl* avl, void* vertex);
void* avl_get_right(const struct avl* avl, void* vertex);

#endif 


