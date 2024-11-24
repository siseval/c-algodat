#include "bst.h"

static void set_parent(struct bst* bst, void* vertex, void* data);
static void set_left(struct bst* bst, void* vertex, void* data);
static void set_right(struct bst* bst, void* vertex, void* data);

static void* get_parent(const struct bst* bst, void* vertex);
static void* get_left(const struct bst* bst, void* vertex);
static void* get_right(const struct bst* bst, void* vertex);

static bool has_parent(const struct bst* bst, void* vertex);
static bool has_left(const struct bst* bst, void* vertex);
static bool has_right(const struct bst* bst, void* vertex);


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
            void* left_child = get_left(bst, cur_vertex);
            if (!has_left(bst, cur_vertex))
            {
                set_left(bst, cur_vertex, data);
                break;
            }
            cur_vertex = left_child;
        }
        else
        {
            void* right_child = get_right(bst, cur_vertex);
            if (!has_right(bst, cur_vertex))
            {
                set_right(bst, cur_vertex, data);
                break;
            }
            cur_vertex = right_child;
        }
    }
    add_vertex(bst, data);
    set_parent(bst, data, cur_vertex);
}

static void* get_min_child(const struct bst* bst, void* vertex)
{
    void* cur_vertex = vertex;
    while (true)
    {
        if (!has_left(bst, cur_vertex))
        {
            return cur_vertex;
        }
        cur_vertex = get_left(bst, cur_vertex);
    }
}

static bool is_left_child(const struct bst* bst, void* vertex)
{
    return has_parent(bst, vertex) && get_left(bst, get_parent(bst, vertex)) == vertex;
}

static bool is_right_child(const struct bst* bst, void* vertex)
{
    return has_parent(bst, vertex) && get_right(bst, get_parent(bst, vertex)) == vertex;
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

    if (is_left_child(bst, data))
    {
        set_left(bst, get_parent(bst, data), NULL);
    }
    if (is_right_child(bst, data))
    {
        set_right(bst, get_parent(bst, data), NULL);
    }

    if (get_parent(bst, vertex) != data)
    {
        set_parent(bst, data, get_parent(bst, vertex));
    }
    if (get_left(bst, vertex) != data)
    {
        set_left(bst, data, get_left(bst, vertex));
    }
    if (get_right(bst, vertex) != data)
    {
        set_right(bst, data, get_right(bst, vertex));
    }

    void* parent = get_parent(bst, vertex);

    set_left(bst, parent, data);
    set_right(bst, parent, data);
    if (is_left_child(bst, vertex))
    {
        set_parent(bst, get_left(bst, vertex), data);
    }
    else if (is_right_child(bst, vertex))
    {
        set_parent(bst, get_right(bst, vertex), data);
    }

    hashset_remove(bst->graph->vertices, vertex);
    list_remove(bst->graph->vertices_list, vertex);
}

void bst_remove(struct bst* bst, void* data)
{
    if (!bst_contains(bst, data))
    {
        return;
    }

    if (has_left(bst, data) && has_right(bst, data))
    {
        void* min_child = get_min_child(bst, get_right(bst, data));
        replace_vertex(bst, data, min_child);
    }
    else if (has_left(bst, data))
    {
        void* left_child = get_left(bst, data);
        replace_vertex(bst, data, left_child);
    }
    else if (has_right(bst, data))
    {
        void* right_child = get_right(bst, data);
        replace_vertex(bst, data, right_child);
    }
    else
    {
        void* parent = get_parent(bst, data);
        if (get_left(bst, parent) == data)
        {
            set_left(bst, parent, NULL);
        }
        else
        {
            set_right(bst, parent, NULL);
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
            if (!has_left(bst, cur_vertex))
            {
                fprintf(stderr, "get: data not found.");
                return NULL;
            }
            cur_vertex = get_left(bst, cur_vertex);
        }
        else if ((int64_t)data > (int64_t)cur_vertex)
        {
            if (!has_right(bst, cur_vertex))
            {
                fprintf(stderr, "get: data not found.");
                return NULL;
            }
            cur_vertex = get_right(bst, cur_vertex);
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
            if (!has_left(bst, cur_vertex))
            {
                return false;
            }
            cur_vertex = get_left(bst, cur_vertex);
        }
        else if ((int64_t)data > (int64_t)cur_vertex)
        {
            if (!has_right(bst, cur_vertex))
            {
                return false;
            }
            cur_vertex = get_right(bst, cur_vertex);
        }
        else
        {
            return true;
        }
    }
}

void* get_min(const struct bst* bst)
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

void set_parent(struct bst* bst, void* vertex, void* data)
{
    set_neighbor(bst, vertex, data, 0);
}

void set_left(struct bst* bst, void* vertex, void* data)
{
    set_neighbor(bst, vertex, data, 1);
}

void set_right(struct bst* bst, void* vertex, void* data)
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

void* get_parent(const struct bst* bst, void* vertex)
{
    return get_neighbor(bst, vertex, 0);
}

void* get_left(const struct bst* bst, void* vertex)
{
    return get_neighbor(bst, vertex, 1);
}

void* get_right(const struct bst* bst, void* vertex)
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

bool has_parent(const struct bst* bst, void* vertex)
{
    return has_neighbor(bst, vertex, 0);
}

bool has_left(const struct bst* bst, void* vertex)
{
    return has_neighbor(bst, vertex, 1);
}

bool has_right(const struct bst* bst, void* vertex)
{
    return has_neighbor(bst, vertex, 2);
}

static void print_int_recursive(const struct bst* bst, void* vertex, const char* prefix, const bool is_left)
{
    printf("\n%s", prefix);
    char next_prefix[128];
    strcpy(next_prefix, prefix);

    if (vertex != bst->root)
    {
        printf("%s", is_left ? "├──" : "└──");
        strcat(next_prefix, is_left ? "│  " : "   ");
    }
    if (vertex == NULL)
    {
        printf("()");
        return;
    }
    printf("%lld", (int64_t)vertex);
    print_int_recursive(bst, get_left(bst, vertex), next_prefix, true);
    print_int_recursive(bst, get_right(bst, vertex), next_prefix, false);
}

void bst_print_int(const struct bst* bst)
{
    print_int_recursive(bst, bst->root, "", false);
}
