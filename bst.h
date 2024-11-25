#ifndef bst_H
#define bst_H

#include <stdlib.h>
#include <stdint.h>
#include "list.h"

#define bst(T) bst_create();

struct bst
{
    uint64_t count;
    struct node* root;
};

struct node
{
    struct node* left;
    struct node* right;
    int64_t data;
};

struct bst* bst_create();
void bst_destroy(struct bst* bst);

void bst_insert(struct bst* bst, int64_t data);
void bst_remove(struct bst* bst, int64_t data);

int64_t bst_get(const struct bst* bst, int64_t data);
bool bst_contains(const struct bst* bst, int64_t data);
int64_t bst_get_min(const struct bst* bst);

struct list* bst_get_range(const struct bst* bst, const int64_t from, const int64_t to);

void bst_print_int(const struct bst* bst);

#endif 


