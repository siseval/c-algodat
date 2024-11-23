#include "bst.h"

struct bst* bst_create()
{
    struct bst* bst = malloc(sizeof(struct bst));
    bst->count = 0;
    bst->root = NULL;
    bst->graph = graph_create(false);
    return bst;
}
void bst_destroy(struct bst* bst)
{
    graph_destroy(bst->graph);
    free(bst);
}

void bst_insert(struct bst* bst, void* data)
{
    if (bst->root == NULL)
    {
        bst->root = data;
        graph_add_vertex(bst->graph, data);
        struct list* vertex_weights = graph_get_vertex_edges(bst->graph, data);
        for (uint8_t i = 0; i < 3; i++)
        {
            list_append(vertex_weights, NULL);
        }
        return;
    }

    void* cur_vertex = bst->root;
    while (true)
    {
        if ((int64_t)data <= (int64_t)cur_vertex)
        {
            void* left_child = bst_get_left(bst, cur_vertex);
            if (left_child == NULL)
            {
                bst_set_left(bst, cur_vertex, data);
                break;
            }
            cur_vertex = left_child;
        }
        else
        {
            void* right_child = bst_get_right(bst, cur_vertex);
            if (right_child == NULL)
            {
                bst_set_right(bst, cur_vertex, data);
                break;
            }
            cur_vertex = right_child;
        }
    }
    graph_add_vertex(bst->graph, data);
    struct list* vertex_weights = graph_get_vertex_edges(bst->graph, data);
    for (uint8_t i = 0; i < 3; i++)
    {
        list_append(vertex_weights, NULL);
    }
    bst_set_parent(bst, data, cur_vertex);
}
void bst_remove(struct bst* bst, void* data);

void* bst_get(const struct bst*, void* data);
void* bst_contains(const struct bst*, void* data);


void bst_set_parent(struct bst* bst, void* vertex, void* data)
{
    struct list* vertex_weights = graph_get_vertex_edges(bst->graph, vertex);
    struct vertex_weight* vertex_weight = malloc(sizeof(struct vertex_weight));
    vertex_weight->vertex = data;
    vertex_weight->weight = 0;
    list_replace(vertex_weights, vertex_weight, 0);
}

void bst_set_left(struct bst* bst, void* vertex, void* data)
{
    struct list* vertex_weights = graph_get_vertex_edges(bst->graph, vertex);
    struct vertex_weight* vertex_weight = malloc(sizeof(struct vertex_weight));
    vertex_weight->vertex = data;
    vertex_weight->weight = 0;
    list_replace(vertex_weights, vertex_weight, 1);
}

void bst_set_right(struct bst* bst, void* vertex, void* data)
{
    struct list* vertex_weights = graph_get_vertex_edges(bst->graph, vertex);
    struct vertex_weight* vertex_weight = malloc(sizeof(struct vertex_weight));
    vertex_weight->vertex = data;
    vertex_weight->weight = 0;
    list_replace(vertex_weights, vertex_weight, 2);
}


void* bst_get_parent(const struct bst* bst, void* vertex)
{
    struct list* vertex_weights = graph_get_vertex_edges(bst->graph, vertex);
    struct vertex_weight* vertex_weight = list_get(vertex_weights, 0);
    if (vertex_weight == NULL)
    {
        return NULL;
    }
    return vertex_weight->vertex;
}

void* bst_get_left(const struct bst* bst, void* vertex)
{
    struct list* vertex_weights = graph_get_vertex_edges(bst->graph, vertex);
    struct vertex_weight* vertex_weight = list_get(vertex_weights, 1);
    if (vertex_weight == NULL)
    {
        return NULL;
    }
    return vertex_weight->vertex;
}

void* bst_get_right(const struct bst* bst, void* vertex)
{
    struct list* vertex_weights = graph_get_vertex_edges(bst->graph, vertex);
    struct vertex_weight* vertex_weight = list_get(vertex_weights, 2);
    if (vertex_weight == NULL)
    {
        return NULL;
    }
    return vertex_weight->vertex;
}

