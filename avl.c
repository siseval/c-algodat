#include "avl.h"

struct avl* avl_create()
{
    struct avl* avl = malloc(sizeof(struct avl));
    avl->tree = bst_create();
    avl->heights = hashmap_create(32, false);
    return avl;
}

void avl_destroy(struct avl* avl)
{
    bst_destroy(avl->tree);
    hashmap_destroy(avl->heights);
    free(avl);
}

static int64_t get_height(struct avl* avl, void* vertex)
{
    if (!hashmap_has_key(avl->heights, vertex))
    {
        return -1;
    }
    return (int64_t)hashmap_get(avl->heights, vertex);
}

static int64_t update_height(struct avl* avl, void* vertex)
{
    int64_t left_height = -1;
    int64_t right_height = -1;
    if (bst_has_left(avl->tree, vertex))
    {
        left_height = update_height(avl, bst_get_left(avl->tree, vertex));
    }
    if (bst_has_right(avl->tree, vertex))
    {
        right_height = update_height(avl, bst_get_right(avl->tree, vertex));
    }

    int64_t height = 1 + (left_height >= right_height ? left_height : right_height);
    hashmap_put(avl->heights, vertex, (void*)height);
    return height;
}

void rotate_left(struct avl* avl, void* vertex)
{
    void* left_vertex = bst_get_left(avl->tree, vertex);
    void* right_vertex = bst_get_right(avl->tree, vertex);
    void* right_left_vertex = bst_get_left(avl->tree, right_vertex);
    bst_replace(avl->tree, vertex, right_vertex);
    bst_replace(avl->tree, left_vertex, vertex);
    bst_replace(avl->tree, right_vertex, right_left_vertex);
}

void avl_insert(struct avl* avl, void* data)
{
    bst_insert(avl->tree, data);
    update_height(avl, avl->tree->root);
}

void avl_remove(struct avl* avl, void* data)
{
    bst_remove(avl->tree, data);
}

void* avl_get(const struct avl* avl, void* data)
{
    return bst_get(avl->tree, data);
}

bool avl_contains(const struct avl* avl, void* data)
{
    return bst_contains(avl->tree, data);
}

struct list* avl_get_range(const struct avl* avl, const int64_t from, const int64_t to)
{
    return bst_get_range(avl->tree, from, to);
}

void avl_print_int(const struct avl* avl)
{
    bst_print_int(avl->tree);
}

