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

void* bst_get(const struct bst* bst, void* data);
bool bst_contains(const struct bst* bst, void* data);
void* bst_get_min(const struct bst* bst);

void bst_print_int(const struct bst* bst);

#endif 


