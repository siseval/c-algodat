#include "avl.h"

struct avl* avl_tree_create()
{
    struct avl* avl = malloc(sizeof(struct avl));
    avl->tree = bst_create();
    return avl;
}

void avl_destroy(struct avl* avl)
{
    bst_destroy(avl->tree);
    free(avl);
}

void avl_insert(struct avl* avl)
{

}
void avl_remove(struct avl* avl);

void* avl_get(const struct avl*, void* vertex);
void* avl_contains(const struct avl*, void* vertex);

void* avl_get_parent(const struct avl* avl, void* vertex);
void* avl_get_left(const struct avl* avl, void* vertex);
void* avl_get_right(const struct avl* avl, void* vertex);

