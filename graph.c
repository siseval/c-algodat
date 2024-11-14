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


static struct list* path_from_parent_map(struct hashmap* parent_map, void* start, void* goal)
{
    struct list* path_list = list_create(32);
    void* cur_vertex = goal;
    while (cur_vertex != start)
    {
        list_append(path_list, cur_vertex);
        cur_vertex = hashmap_get(parent_map, cur_vertex);
    }
    list_append(path_list, start);
    return path_list;
}

struct list* graph_shortest_path(struct graph* graph, void* start, void* goal)
{
    struct hashset* visited = hashset_create(32, false);
    struct hashmap* parents = hashmap_create(32, false);
    struct queue* to_visit = queue_create(32);

    queue_enqueue(to_visit, start);
    bool goal_found = false;
    while (to_visit->count > 0)
    {
        void* cur_vertex = queue_dequeue(to_visit);
        hashset_put(visited, cur_vertex);
        struct list* cur_vertex_neighbours = graph_get_vertex_neighbours(graph, cur_vertex);

        for (uint64_t i = 0 ; i < cur_vertex_neighbours->count; i++)
        {
            void* neighbor = list_get(cur_vertex_neighbours, i);
            if (!hashset_contains(visited, neighbor))
            {
                hashmap_put(parents, neighbor, cur_vertex);
                queue_enqueue(to_visit, neighbor);
            }
            if (neighbor == goal)
            {
                goal_found = true;
                break;
            }
        }
        list_destroy(cur_vertex_neighbours);
        if (goal_found)
        {
            break;
        }
    }

    if (!goal_found)
    {
        fprintf(stderr, "graph_shortest_path: path not found.");
        return NULL;
    }

    struct list* path_list = path_from_parent_map(parents, start, goal);
    hashset_destroy(visited);
    hashmap_destroy(parents);
    queue_destroy(to_visit);
    return path_list;
}


struct list* graph_get_vertex_neighbours(struct graph* graph, void* vertex)
{
    return hashmap_get(graph->edges, vertex);
}

void graph_print_int(struct graph* graph)

{
    for (uint64_t i = 0; i < graph->num_vertices; i++)
    {
        void* vertex = list_get(graph->vertices_list, i);
        printf("\n%lld => ", (uint64_t)vertex);
        list_print_char(graph_get_vertex_neighbours(graph, vertex));
    }
}

void graph_print_char(struct graph* graph)

{
    for (uint64_t i = 0; i < graph->num_vertices; i++)
    {
        void* vertex = list_get(graph->vertices_list, i);
        printf("\n%c => ", (char)(uint64_t)vertex);
        list_print_char(graph_get_vertex_neighbours(graph, vertex));
    }
}

