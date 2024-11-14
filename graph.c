#include "graph.h"


struct graph* graph_create(void)
{
    struct graph* graph = malloc(sizeof(struct graph));

    graph->vertices = hashset_create(32, false);
    graph->edges = hashmap_create(32, false);
    graph->weights = hashmap_create(32, false);

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
}

void graph_add_edge(struct graph* graph, void* from, void* to)
{
    hashmap_put(graph->edges, from, to);
}

void graph_add_weighted_edge(struct graph* graph, void* from, void* to, const int64_t weight)
{
    hashmap_put(graph->edges, from, to);
    graph_update_weight(graph, from, to, weight);
}

void graph_update_weight(struct graph* graph, void* from, void* to, const int64_t weight)
{
    void* edge[] = { from, to };
    hashmap_put(graph->weights, edge, (void*)weight);
}
