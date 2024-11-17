#include "graph.h"


struct graph* graph_create(const bool is_directed)
{
    struct graph* graph = malloc(sizeof(struct graph));

    graph->is_directed = is_directed;
    graph->vertices = hashset_create(32, false);
    graph->edges = hashmap_create(32, false);
    graph->vertices_list = list_create(32);

    return graph;
}

void graph_destroy(struct graph* graph)
{
    for (uint64_t i = 0; i < graph->num_vertices; i++)
    {
        struct list* vertex_weights = graph_get_vertex_edges(graph, list_get(graph->vertices_list, i));
        for (uint64_t i = 0; i < vertex_weights->count; i++)
        {
            free(list_get(vertex_weights, i));
        }
        list_destroy(vertex_weights);
    }
    hashset_destroy(graph->vertices);
    hashmap_destroy(graph->edges);
    free(graph);
}


void graph_add_vertex(struct graph* graph, void* value)
{
    hashset_put(graph->vertices, value);
    list_append(graph->vertices_list, value);
    struct list* connections = list_create(8);
    hashmap_put(graph->edges, value, connections);
    graph->num_vertices++;
}

void graph_add_edge(struct graph* graph, void* from, void* to)
{
    if (!hashset_contains(graph->vertices, from))
    {
        graph_add_vertex(graph, from);
    }
    if (!hashset_contains(graph->vertices, to))
    {
        graph_add_vertex(graph, to);
    }

    struct vertex_weight* to_weight = malloc(sizeof(struct vertex_weight));
    to_weight->vertex = to;
    to_weight->weight = 0;

    list_append(hashmap_get(graph->edges, from), to_weight);
    if (!graph->is_directed)
    {
        struct vertex_weight* from_weight = malloc(sizeof(struct vertex_weight));
        from_weight->vertex = from;
        from_weight->weight = 0;
        list_append(hashmap_get(graph->edges, to), from_weight);
    }
    graph->num_edges++;
}

void graph_add_weighted_edge(struct graph* graph, void* from, void* to, const int64_t weight)
{
    graph_add_edge(graph, from, to);
    graph_update_weight(graph, from, to, weight);
}

void graph_update_weight(struct graph* graph, void* from, void* to, const int64_t weight)
{
    struct list* from_neighbors = graph_get_vertex_edges(graph, from);
    for (uint64_t i = 0; i < from_neighbors->count; i++)
    {
        struct vertex_weight* to_weight = (struct vertex_weight*)list_get(from_neighbors, i);
        if (to_weight->vertex == to)
        {
            to_weight->weight = weight;
        }
    }

    if (graph->is_directed)
    {
        return;
    }

    struct list* to_neighbors = graph_get_vertex_edges(graph, to);
    for (uint64_t i = 0; i < to_neighbors->count; i++)
    {
        struct vertex_weight* from_weight = (struct vertex_weight*)list_get(to_neighbors, i);
        if (from_weight->vertex == from)
        {
            from_weight->weight = weight;
        }
    }
}


static struct vertex_weight* get_vertex_weight(const struct graph* graph, void* from, void* to)
{
    struct list* vertex_weights = graph_get_vertex_edges(graph, from);
    for (uint64_t i = 0; i < vertex_weights->count; i++)
    {
        struct vertex_weight* vertex_weight = list_get(vertex_weights, i);
        if (vertex_weight->vertex == to)
        {
            return vertex_weight;
        }
    }
    fprintf(stderr, "get_vertex_weight: vertex_weight not found.\n");
    return NULL;
}

uint64_t graph_get_edge_weight(const struct graph* graph, void* from, void* to)
{
    struct vertex_weight* vertex_weight = get_vertex_weight(graph, from, to);
    if (vertex_weight == NULL)
    {
        fprintf(stderr, "graph_get_weight: edge does not exist.\n");
        return 0;
    }
    return vertex_weight->weight;
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
    return list_reverse(path_list);
}


uint64_t graph_path_weight(const struct graph* graph, const struct list* path)
{
    uint64_t accumulated_weight = 0;
    for (uint64_t i = 0; i < path->count - 1; i++)
    {
        uint64_t edge_weight = graph_get_edge_weight(graph, list_get(path, i), list_get(path, i + 1));
        accumulated_weight += edge_weight;
    }
    return accumulated_weight;
}


struct list* graph_shortest_path(const struct graph* graph, void* start, void* goal)
{
    struct hashset* visited = hashset_create(32, false);
    struct hashmap* parents = hashmap_create(32, false);
    struct queue* to_visit = queue_create(32);

    queue_enqueue(to_visit, start);
    bool goal_found = false;
    while (to_visit->count > 0)
    {
        void* cur_vertex = queue_dequeue(to_visit);
        struct list* cur_vertex_neighbors = graph_get_vertex_edges(graph, cur_vertex);

        for (uint64_t i = 0 ; i < cur_vertex_neighbors->count; i++)
        {
            struct vertex_weight* neighbor_weight = list_get(cur_vertex_neighbors, i);
            void* neighbor = neighbor_weight->vertex;
            if (!hashset_contains(visited, neighbor))
            {
                hashmap_put(parents, neighbor, cur_vertex);
                queue_enqueue(to_visit, neighbor);
                hashset_put(visited, neighbor);
            }
            if (neighbor == goal)
            {
                goal_found = true;
                break;
            }
        }
        if (goal_found)
        {
            break;
        }
    }

    struct list* path_list = NULL;

    if (!goal_found)
    {
        fprintf(stderr, "graph_shortest_path: path not found.\n");
    }
    else
    {
        path_list = path_from_parent_map(parents, start, goal);
    }

    hashset_destroy(visited);
    hashmap_destroy(parents);
    queue_destroy(to_visit);
    return path_list;
}

struct list* graph_lightest_path(const struct graph* graph, void* start, void* goal)
{
    struct hashmap* parents = hashmap_create(32, false);
    struct heap* to_visit = heap_create(32, true);
    struct hashmap* distances = hashmap_create(32, false);
    
    for (uint64_t i = 0; i < graph->num_vertices; i++)
    {
        hashmap_put(distances, list_get(graph->vertices_list, i), (uint64_t*)INT_MAX);
    }
    hashmap_put(distances, start, (uint64_t*)0);

    void** distance_start = malloc(sizeof(void*) * 2);
    distance_start[0] = (uint64_t*)0;
    distance_start[1] = start;
    heap_push(to_visit, distance_start);
    bool goal_found = false;

    while (to_visit->count > 0)
    {
        void** distance_vertex = heap_pop(to_visit);
        void* cur_vertex = distance_vertex[1];
        free(distance_vertex);
        if (cur_vertex == goal)
        {
            goal_found = true;
            break;
        }
        uint64_t cur_distance = (uint64_t)hashmap_get(distances, cur_vertex);
        struct list* cur_vertex_neighbors = graph_get_vertex_edges(graph, cur_vertex);
        
        for (uint64_t i = 0; i < cur_vertex_neighbors->count; i++)
        {
            struct vertex_weight* neighbor_weight = list_get(cur_vertex_neighbors, i);
            void* neighbor = neighbor_weight->vertex;
            uint64_t weight = neighbor_weight->weight;

            uint64_t distance = cur_distance + weight;
            if (distance >= (uint64_t)hashmap_get(distances, neighbor))
            {
                continue;
            }

            hashmap_put(parents, neighbor, cur_vertex);
            hashmap_put(distances, neighbor, (uint64_t*)distance);
            void** distance_neighbor = malloc(sizeof(void*) * 2);
            distance_neighbor[0] = (uint64_t*)distance;
            distance_neighbor[1] = neighbor;
            heap_push(to_visit, distance_neighbor);
        }
        if (goal_found)
        {
            break;
        }
    }

    for (uint64_t i = 0; i < to_visit->count; i++)
    {
        free(heap_pop(to_visit));
    }

    struct list* path_list = NULL;
    if (!goal_found)
    {
        fprintf(stderr, "graph_lightest_path: path not found.\n");
    }
    else
    {
        path_list = path_from_parent_map(parents, start, goal);;
    }

    hashmap_destroy(parents);
    hashmap_destroy(distances);
    heap_destroy(to_visit);
    return path_list;
}

struct graph* graph_min_spanning_tree(const struct graph* graph)
{
    struct graph* spanning_tree = graph_create(true);
    struct heap* to_visit = heap_create(32, true);

    void** distance_start_null = malloc(sizeof(void*) * 3);
    void* start = hashset_get_random(graph->vertices);
    distance_start_null[0] = (uint64_t*)0;
    distance_start_null[1] = start;
    distance_start_null[2] = NULL;

    heap_push(to_visit, distance_start_null);

    while (to_visit->count > 0)
    {
        void** weight_vertex_parent = heap_pop(to_visit);
        uint64_t weight = (uint64_t)weight_vertex_parent[0];
        void* cur_vertex = weight_vertex_parent[1];
        void* parent = weight_vertex_parent[2];
        free(weight_vertex_parent);

        if (hashmap_has_key(spanning_tree->edges, cur_vertex))
        {
            continue;
        }
        if (cur_vertex != start)
        {
            graph_add_weighted_edge(spanning_tree, cur_vertex, parent, weight);
        }

        struct list* cur_vertex_neighbors = graph_get_vertex_edges(graph, cur_vertex);

        for (uint64_t i = 0; i < cur_vertex_neighbors->count; i++)
        {
            struct vertex_weight* neighbor_weight = list_get(cur_vertex_neighbors, i);
            if (neighbor_weight->vertex == start)
            {
                continue;
            }
            void** weight_neighbor_parent = malloc(sizeof(void*) * 3);
            weight_neighbor_parent[0] = (uint64_t*)graph_get_edge_weight(graph, cur_vertex, neighbor_weight->vertex);
            weight_neighbor_parent[1] = neighbor_weight->vertex;
            weight_neighbor_parent[2] = cur_vertex;
            heap_push(to_visit, weight_neighbor_parent);
        }
    }

    heap_destroy(to_visit);
    return spanning_tree; 
}

struct list* graph_get_vertex_edges(const struct graph* graph, void* vertex)
{
    return hashmap_get(graph->edges, vertex);
}


struct graph* graph_reverse_direction(struct graph* graph)
{
    if (!graph->is_directed)
    {
        fprintf(stderr, "graph_reverse_direction: graph is not directed.");
        return graph;
    }

    struct graph* reversed_graph = graph_create(true);
    for (uint64_t i = 0; i < graph->num_vertices; i++)
    {
        void* vertex = list_get(graph->vertices_list, i);
        struct list* vertex_neighbor_weights = graph_get_vertex_edges(graph, vertex);
        for (uint64_t j = 0; j < vertex_neighbor_weights->count; j++)
        {
            struct vertex_weight* neighbor_weight = list_get(vertex_neighbor_weights, j);
            graph_add_weighted_edge(reversed_graph, neighbor_weight->vertex, vertex, neighbor_weight->weight);
        }
    }
    free(graph);
    graph = reversed_graph;
    return graph;
}


void graph_print_vertex_neighbors_char(const struct graph* graph, void* vertex)
{
    struct list* vertex_neigbor_weights = graph_get_vertex_edges(graph, vertex);
    printf("(");
    for (uint64_t i = 0; i < vertex_neigbor_weights->count; i++)
    {
        char neighbor_char = (char)(uint64_t)((struct vertex_weight*)list_get(vertex_neigbor_weights, i))->vertex;
        printf(i == vertex_neigbor_weights->count - 1 ? "%c" : "%c, ", neighbor_char);
    }
    printf(")");
}

void graph_print_vertex_neighbors_int(const struct graph* graph, void* vertex)
{
    struct list* vertex_neigbor_weights = graph_get_vertex_edges(graph, vertex);
    printf("(");
    for (uint64_t i = 0; i < vertex_neigbor_weights->count; i++)
    {
        uint64_t neighbor_char = (uint64_t)((struct vertex_weight*)list_get(vertex_neigbor_weights, i))->vertex;
        printf(i == vertex_neigbor_weights->count - 1 ? "%lld" : "%lld, ", neighbor_char);
    }
    printf(")");
}

void graph_print_int(const struct graph* graph)
{
    for (uint64_t i = 0; i < graph->num_vertices; i++)
    {
        void* vertex = list_get(graph->vertices_list, i);
        printf("%lld => ", (uint64_t)vertex);
        graph_print_vertex_neighbors_int(graph, vertex);
        printf("\n");
    }
}

void graph_print_char(const struct graph* graph)
{
    for (uint64_t i = 0; i < graph->num_vertices; i++)
    {
        void* vertex = list_get(graph->vertices_list, i);
        printf("%c => ", (char)(uint64_t)vertex);
        graph_print_vertex_neighbors_char(graph, vertex);
        printf("\n");
    }
}

