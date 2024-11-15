#ifndef GRAPH_H
#define GRAPH_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "list.h"
#include "hashmap.h"
#include "hashset.h"
#include "queue.h"

#define graph(T) graph_create(false)
#define directed_graph(T) graph_create(true)

struct graph
{
    bool is_directed;
    uint64_t num_vertices;
    uint64_t num_edges;
    struct hashset* vertices;
    struct hashmap* edges;
    struct hashmap* weights;
    struct list* vertices_list;
};

struct graph* graph_create(const bool is_directed);
void graph_destroy(struct graph* graph);

void graph_add_vertex(struct graph* graph, void* value);
void graph_add_edge(struct graph* graph, void* from, void* to);
void graph_add_weighted_edge(struct graph* graph, void* from, void* to, const int64_t weight);
void graph_update_weight(struct graph* graph, void* from, void* to, const int64_t weight);

struct list* graph_shortest_path(struct graph* graph, void* start, void* goal);

struct list* graph_get_vertex_neighbors(struct graph* graph, void* vertex);

void graph_print_int(struct graph* graph);
void graph_print_char(struct graph* graph);

#endif

