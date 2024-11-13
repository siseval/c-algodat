#include "graph.h"
#include "hashset.h"


struct graph* graph_create(void)
{
    struct graph* graph = malloc(sizeof(struct graph));

    graph->vertices = hashset_create(32, false);
    graph->edges = hashmap_create(32, false);

    return graph;
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
    void* edge[] = { from, to };
    hashmap_put(graph->weights, edge, (void*)weight);
}
