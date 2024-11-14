#ifndef GRAPH_H
#define GRAPH_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "hashmap.h"
#include "hashset.h"

struct graph
{
    struct hashset* vertices;
    struct hashmap* edges;
    struct hashmap* weights;
};

struct graph* graph_create(void);
void graph_destroy(struct graph* graph);

void graph_add_vertex(struct graph* graph, void* value);
void graph_add_edge(struct graph* graph, void* from, void* to);
void graph_add_weighted_edge(struct graph* graph, void* from, void* to, const int64_t weight);
void graph_update_weight(struct graph* graph, void* from, void* to, const int64_t weight);

#endif

