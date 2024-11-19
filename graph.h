#ifndef GRAPH_H
#define GRAPH_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include "list.h"
#include "hashmap.h"
#include "hashset.h"
#include "stack.h"
#include "queue.h"
#include "heap.h"

#define graph(T) graph_create(false)
#define directed_graph(T) graph_create(true)

struct graph
{
    bool is_directed;
    uint64_t num_vertices;
    uint64_t num_edges;
    struct hashset* vertices;
    struct hashmap* edges;
    struct list* vertices_list;
};

struct vertex_weight
{
    void* vertex;
    uint64_t weight;
};


struct graph* graph_create(const bool is_directed);
void graph_destroy(struct graph* graph);

void graph_add_vertex(struct graph* graph, void* value);
void graph_add_edge(struct graph* graph, void* from, void* to);
void graph_add_weighted_edge(struct graph* graph, void* from, void* to, const int64_t weight);
void graph_update_weight(struct graph* graph, void* from, void* to, const int64_t weight);

void graph_remove_vertex(struct graph* graph, void* vertex);
void graph_remove_edge(struct graph* graph, void* from, void* to);

uint64_t graph_get_edge_weight(const struct graph* graph, void* from, void* to);

uint64_t graph_path_weight(const struct graph* graph, const struct list* path);

struct list* graph_shortest_path(const struct graph* graph, void* start_vertex, void* goal_vertex);
struct list* graph_lightest_path(const struct graph* graph, void* start_vertex, void* goal_vertex);
struct graph* graph_min_spanning_tree(const struct graph* graph);
struct graph* graph_spanning_tree(const struct graph* graph);
struct list* graph_all_reachable_from(const struct graph* graph, void* start_vertex);
struct list* graph_separation_vertices(const struct graph* graph);
struct stack* graph_topological_sort(const struct graph* graph);
struct list* graph_strongly_connected_components(const struct graph* graph);

bool graph_is_biconnected(const struct graph* graph);

struct list* graph_get_vertex_edges(const struct graph* graph, void* vertex);
struct vertex_weight* graph_get_vertex_edge(const struct graph* graph, void* from, void* to);

struct graph* graph_get_reverse(const struct graph* graph);
struct graph* graph_reverse_direction(struct graph* graph);

void graph_print_vertex_neighbors_char(const struct graph* graph, void* vertex);
void graph_print_vertex_neighbors_int(const struct graph* graph, void* vertex);
void graph_print_int(const struct graph* graph);
void graph_print_char(const struct graph* graph);

#endif
