#include "graph.h"


struct graph* graph_create(void)
{
    struct graph* graph = malloc(sizeof(struct graph));

    graph->vertices = hashset_create(32, false);
    graph->edges = hashmap_create(32, false);
    graph->weights = hashmap_create(32, false);
    graph->vertices_list = list_create(32);

    return graph;
}

void graph_destroy(struct graph* graph)
{
    hashset_destroy(graph->vertices);
    hashmap_destroy(graph->edges);
    hashmap_destroy(graph->weights);
    free(graph);
}


void graph_add_vertex(struct graph* graph, void* value)
{
    hashset_put(graph->vertices, value);
    list_append(graph->vertices_list, value);
    struct list* connections = list_create(32);
    hashmap_put(graph->edges, value, connections);
    graph->num_vertices++;
}

void graph_add_edge(struct graph* graph, void* from, void* to)
{
    list_append(hashmap_get(graph->edges, from), to);
    graph->num_edges++;
}

void graph_add_weighted_edge(struct graph* graph, void* from, void* to, const int64_t weight)
{
    graph_add_edge(graph, from, to);
    graph_update_weight(graph, from, to, weight);
}

void graph_update_weight(struct graph* graph, void* from, void* to, const int64_t weight)
{
    void* edge[] = { from, to };
    hashmap_put(graph->weights, edge, (void*)weight);
}


struct list* graph_get_vertex_edges(struct graph* graph, void* vertex)
{
    return hashmap_get(graph->edges, vertex);
}

void graph_print_int(struct graph* graph)

{
    for (uint64_t i = 0; i < graph->num_vertices; i++)
    {
        void* vertex = list_get(graph->vertices_list, i);
        printf("\n%lld => ", (uint64_t)vertex);
        list_print_char(graph_get_vertex_edges(graph, vertex));
    }
}

void graph_print_char(struct graph* graph)

{
    for (uint64_t i = 0; i < graph->num_vertices; i++)
    {
        void* vertex = list_get(graph->vertices_list, i);
        printf("\n%c => ", (char)(uint64_t)vertex);
        list_print_char(graph_get_vertex_edges(graph, vertex));
    }
}

