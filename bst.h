#ifndef bst_H
#define bst_H

#include "graph.h"

#define bst(T) bst_create();

struct bst
{
    uint64_t count;
    void* root;
    struct graph* graph;
};

struct bst* bst_create();
void bst_destroy(struct bst* bst);

void bst_insert(struct bst* bst, void* data);
void bst_remove(struct bst* bst, void* data);

void* bst_get(const struct bst*, void* data);
void* bst_contains(const struct bst*, void* data);

void bst_set_parent(struct bst* bst, void* vertex, void* data);
void bst_set_left(struct bst* bst, void* vertex, void* data);
void bst_set_right(struct bst* bst, void* vertex, void* data);

void* bst_get_parent(const struct bst* bst, void* vertex);
void* bst_get_left(const struct bst* bst, void* vertex);
void* bst_get_right(const struct bst* bst, void* vertex);

#endif 


