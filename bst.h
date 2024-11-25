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
void bst_replace(struct bst* bst, void* vertex, void* data);

void* bst_get(const struct bst* bst, void* data);
bool bst_contains(const struct bst* bst, void* data);
void* bst_get_min(const struct bst* bst);
struct list* bst_get_range(const struct bst* bst, const int64_t from, const int64_t to);

void bst_set_parent(struct bst* bst, void* vertex, void* data);
void bst_set_left(struct bst* bst, void* vertex, void* data);
void bst_set_right(struct bst* bst, void* vertex, void* data);

void* bst_get_parent(const struct bst* bst, void* vertex);
void* bst_get_left(const struct bst* bst, void* vertex);
void* bst_get_right(const struct bst* bst, void* vertex);

bool bst_has_parent(const struct bst* bst, void* vertex);
bool bst_has_left(const struct bst* bst, void* vertex);
bool bst_has_right(const struct bst* bst, void* vertex);

void bst_print_int(const struct bst* bst);

#endif 


