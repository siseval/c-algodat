#ifndef BST_H
#define BST_H

#include <stdlib.h>
#include <stdint.h>
#include "list.h"

#define bst(T) bst_create(false);
#define avl(T) bst_create(true);

struct bst
{
    bool is_avl;
    struct node* root;
};

struct node
{
    struct node* left;
    struct node* right;
    int64_t data;
    int16_t height;
};


struct bst* bst_create(const bool is_avl);
void bst_destroy(struct bst* bst);

void bst_insert(struct bst* bst, int64_t data);
void bst_remove(struct bst* bst, int64_t data);

int64_t bst_get(const struct bst* bst, int64_t data);
bool bst_contains(const struct bst* bst, int64_t data);
int64_t bst_get_min(const struct bst* bst);

struct list* bst_get_range(const struct bst* bst, const int64_t from, const int64_t to);
int64_t bst_get_diameter(const struct bst* bst);

void bst_print_int(const struct bst* bst);

#endif 


