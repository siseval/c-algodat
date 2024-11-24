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

static void add_vertex(struct bst* bst, void* data)
{
    graph_add_vertex(bst->graph, data);
    struct list* vertex_weights = graph_get_vertex_edges(bst->graph, data);
    for (uint8_t i = 0; i < 3; i++)
    {
        list_append(vertex_weights, NULL);
    }
}

void bst_insert(struct bst* bst, void* data)
{
    if (bst->root == NULL)
    {
        bst->root = data;
        add_vertex(bst, data);
        return;
    }

    void* cur_vertex = bst->root;
    while (true)
    {
        if ((int64_t)data <= (int64_t)cur_vertex)
        {
            void* left_child = bst_get_left(bst, cur_vertex);
            if (!bst_has_left(bst, cur_vertex))
            {
                bst_set_left(bst, cur_vertex, data);
                break;
            }
            cur_vertex = left_child;
        }
        else
        {
            void* right_child = bst_get_right(bst, cur_vertex);
            if (!bst_has_right(bst, cur_vertex))
            {
                bst_set_right(bst, cur_vertex, data);
                break;
            }
            cur_vertex = right_child;
        }
    }
    add_vertex(bst, data);
    bst_set_parent(bst, data, cur_vertex);
}

static void* get_min_child(const struct bst* bst, void* vertex)
{
    void* cur_vertex = vertex;
    while (true)
    {
        if (!bst_has_left(bst, cur_vertex))
        {
            return cur_vertex;
        }
        cur_vertex = bst_get_left(bst, cur_vertex);
    }
}

static void replace_vertex(struct bst* bst, void* vertex, void* data)
{
    if (vertex == data)
    {
        return;
    }
    if (vertex == bst->root)
    {
        bst->root = data;
    }
    if (!bst_contains(bst, data))
    {
        add_vertex(bst, data);
    }

    if (bst_has_parent(bst, vertex) && bst_get_parent(bst, vertex) != data)
    {
        bst_set_parent(bst, data, bst_get_parent(bst, vertex));
    }
    if (bst_has_left(bst, vertex) && bst_get_left(bst, vertex) != data)
    {
        bst_set_left(bst, data, bst_get_left(bst, vertex));
    }
    if (bst_has_right(bst, vertex) && bst_get_right(bst, vertex) != data)
    {
        bst_set_right(bst, data, bst_get_right(bst, vertex));
    }

    void* parent = bst_get_parent(bst, vertex);
    if (bst_get_left(bst, parent) == vertex)
    {
        bst_set_left(bst, parent, data);
    }
    else if (bst_get_right(bst, parent) == vertex)
    {
        bst_set_right(bst, parent, data);
    }
    if (bst_has_left(bst, vertex))
    {
        bst_set_parent(bst, bst_get_left(bst, vertex), data);
    }
    if (bst_has_right(bst, vertex))
    {
        bst_set_parent(bst, bst_get_right(bst, vertex), data);
    }
    hashset_remove(bst->graph->vertices, vertex);
    list_remove(bst->graph->vertices_list, vertex);
}

void bst_remove(struct bst* bst, void* data)
{
    if (bst_has_left(bst, data) && bst_has_right(bst, data))
    {
        void* min_child = get_min_child(bst, bst_get_right(bst, data));
        replace_vertex(bst, data, min_child);
    }
    else if (bst_has_left(bst, data))
    {
        void* left_child = bst_get_left(bst, data);
        replace_vertex(bst, data, left_child);
    }
    else if (bst_has_right(bst, data))
    {
        void* right_child = bst_get_right(bst, data);
        replace_vertex(bst, data, right_child);
    }
    else
    {
        void* parent = bst_get_parent(bst, data);
        if (bst_get_left(bst, parent) == data)
        {
            bst_set_left(bst, parent, NULL);
        }
        else
        {
            bst_set_right(bst, parent, NULL);
        }
    }
}

void* bst_get(const struct bst* bst, void* data)
{
    void* cur_vertex = bst->root;
    while (true)
    {
        if ((int64_t)data < (int64_t)cur_vertex)
        {
            if (!bst_has_left(bst, cur_vertex))
            {
                fprintf(stderr, "bst_get: data not found.");
                return NULL;
            }
            cur_vertex = bst_get_left(bst, cur_vertex);
        }
        else if ((int64_t)data > (int64_t)cur_vertex)
        {
            if (!bst_has_right(bst, cur_vertex))
            {
                fprintf(stderr, "bst_get: data not found.");
                return NULL;
            }
            cur_vertex = bst_get_right(bst, cur_vertex);
        }
        else
        {
            return data;
        }
    }
}

bool bst_contains(const struct bst* bst, void* data)
{
    void* cur_vertex = bst->root;
    while (true)
    {
        if ((int64_t)data < (int64_t)cur_vertex)
        {
            if (!bst_has_left(bst, cur_vertex))
            {
                return false;
            }
            cur_vertex = bst_get_left(bst, cur_vertex);
        }
        else if ((int64_t)data > (int64_t)cur_vertex)
        {
            if (!bst_has_right(bst, cur_vertex))
            {
                return false;
            }
            cur_vertex = bst_get_right(bst, cur_vertex);
        }
        else
        {
            return true;
        }
    }
}

void* bst_get_min(const struct bst* bst)
{
    return get_min_child(bst, bst->root);
}

static void set_neighbor(struct bst* bst, void* vertex, void* data, const uint8_t index)
{
    struct list* vertex_weights = graph_get_vertex_edges(bst->graph, vertex);
    if (data == NULL)
    {
        list_replace(vertex_weights, NULL, index);
        return;
    }
    struct vertex_weight* vertex_weight = malloc(sizeof(struct vertex_weight));
    vertex_weight->vertex = data;
    vertex_weight->weight = 0;
    list_replace(vertex_weights, vertex_weight, index);
}

void bst_set_parent(struct bst* bst, void* vertex, void* data)
{
    set_neighbor(bst, vertex, data, 0);
}

void bst_set_left(struct bst* bst, void* vertex, void* data)
{
    set_neighbor(bst, vertex, data, 1);
}

void bst_set_right(struct bst* bst, void* vertex, void* data)
{
    set_neighbor(bst, vertex, data, 2);
}


static void* get_neighbor(const struct bst* bst, void* vertex, const uint8_t index)
{
    struct list* vertex_weights = graph_get_vertex_edges(bst->graph, vertex);
    struct vertex_weight* vertex_weight = list_get(vertex_weights, index);
    if (vertex_weight == NULL)
    {
        return NULL;
    }
    return vertex_weight->vertex;
}

void* bst_get_parent(const struct bst* bst, void* vertex)
{
    return get_neighbor(bst, vertex, 0);
}

void* bst_get_left(const struct bst* bst, void* vertex)
{
    return get_neighbor(bst, vertex, 1);
}

void* bst_get_right(const struct bst* bst, void* vertex)
{
    return get_neighbor(bst, vertex, 2);
}


static bool has_neighbor(const struct bst* bst, void* vertex, const uint8_t index)
{
    struct list* vertex_weights = graph_get_vertex_edges(bst->graph, vertex);
    struct vertex_weight* vertex_weight = list_get(vertex_weights, index);
    if (vertex_weight == NULL)
    {
        return false;
    }
    return true;
}

bool bst_has_parent(const struct bst* bst, void* vertex)
{
    return has_neighbor(bst, vertex, 0);
}

bool bst_has_left(const struct bst* bst, void* vertex)
{
    return has_neighbor(bst, vertex, 1);
}

bool bst_has_right(const struct bst* bst, void* vertex)
{
    return has_neighbor(bst, vertex, 2);
}

static void print_int_recursive(const struct bst* bst, void* vertex, const char* prefix, const bool is_left)
{
    printf("\n%s", prefix);
    printf("%s", is_left ? "├──" : "└──");
    if (vertex == NULL)
    {
        printf("()");
        return;
    }
    printf("%lld", (int64_t)vertex);
    char next_prefix[128];
    strcpy(next_prefix, prefix);
    strcat(next_prefix, is_left ? "│  " : "   ");
    print_int_recursive(bst, bst_get_left(bst, vertex), next_prefix, true);
    print_int_recursive(bst, bst_get_right(bst, vertex), next_prefix, false);
}

void bst_print_int(const struct bst* bst)
{
    print_int_recursive(bst, bst->root, "", false);
}
