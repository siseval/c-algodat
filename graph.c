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


void graph_remove_vertex(struct graph* graph, void* vertex)
{
    struct list* vertex_weights = graph_get_vertex_edges(graph, vertex);
    for (uint64_t i = 0; i < vertex_weights->count; i++)
    {
        struct vertex_weight* vertex_weight = list_get(vertex_weights, i);
        graph_remove_edge(graph, vertex, vertex_weight->vertex);
    }

    for (uint64_t i = 0; i < graph->num_vertices; i++)
    {
        void* cur_vertex = list_get(graph->vertices_list, i);
        struct list* vertex_weights = graph_get_vertex_edges(graph, cur_vertex);
        for (uint64_t i = 0; i < vertex_weights->count; i++)
        {
            struct vertex_weight* vertex_weight = list_get(vertex_weights, i);
            if (vertex_weight->vertex == vertex)
            {
                graph_remove_edge(graph, cur_vertex, vertex);
            }
        }
    }

    list_remove(graph->vertices_list, vertex);
    hashset_remove(graph->vertices, vertex);
    graph->num_vertices--;
}

void graph_remove_edge(struct graph* graph, void* from, void* to)
{
    struct vertex_weight* to_weight = graph_get_vertex_edge(graph, from, to);
    list_remove(graph_get_vertex_edges(graph, from), to_weight);
    free(to_weight);
    graph->num_edges--;
    if (graph->is_directed)
    {
        return;
    }
    struct vertex_weight* from_weight = graph_get_vertex_edge(graph, to, from);
    free(from_weight);
    list_remove(graph_get_vertex_edges(graph, to), from_weight);
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

bool graph_has_edge(const struct graph* graph, void* from, void* to)
{
    if (!hashset_contains(graph->vertices, from))
    {
        return false;
    }
    struct list* vertex_weights = graph_get_vertex_edges(graph, from);
    for (uint64_t i = 0; i < vertex_weights->count; i++)
    {
        struct vertex_weight* vertex_weight = list_get(vertex_weights, i);
        if (vertex_weight->vertex == to)
        {
            return true;
        }
    }
    return false;
}

static struct list* path_from_parent_map(struct hashmap* parent_map, void* start_vertex, void* goal_vertex)
{
    struct list* path_list = list_create(32);
    void* cur_vertex = goal_vertex;
    while (cur_vertex != start_vertex)
    {
        list_append(path_list, cur_vertex);
        cur_vertex = hashmap_get(parent_map, cur_vertex);
    }
    list_append(path_list, start_vertex);
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


struct list* graph_shortest_path(const struct graph* graph, void* start_vertex, void* goal_vertex)
{
    struct hashset* visited = hashset_create(32, false);
    struct hashmap* parents = hashmap_create(32, false);
    struct queue* to_visit = queue_create(32);

    queue_enqueue(to_visit, start_vertex);
    bool goal_vertex_found = false;
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
            if (neighbor == goal_vertex)
            {
                goal_vertex_found = true;
                break;
            }
        }
        if (goal_vertex_found)
        {
            break;
        }
    }

    struct list* path_list = NULL;

    if (!goal_vertex_found)
    {
        fprintf(stderr, "graph_shortest_path: path not found.\n");
    }
    else
    {
        path_list = path_from_parent_map(parents, start_vertex, goal_vertex);
    }

    hashset_destroy(visited);
    hashmap_destroy(parents);
    queue_destroy(to_visit);
    return path_list;
}

struct list* graph_lightest_path(const struct graph* graph, void* start_vertex, void* goal_vertex)
{
    struct hashmap* parents = hashmap_create(32, false);
    struct heap* to_visit = heap_create(32, true);
    struct hashmap* distances = hashmap_create(32, false);
    
    for (uint64_t i = 0; i < graph->num_vertices; i++)
    {
        hashmap_put(distances, list_get(graph->vertices_list, i), (uint64_t*)INT_MAX);
    }
    hashmap_put(distances, start_vertex, (uint64_t*)0);

    void** distance_start = malloc(sizeof(void*) * 2);
    distance_start[0] = (uint64_t*)0;
    distance_start[1] = start_vertex;
    heap_push(to_visit, distance_start);
    bool goal_vertex_found = false;

    while (to_visit->count > 0)
    {
        void** distance_vertex = heap_pop(to_visit);
        void* cur_vertex = distance_vertex[1];
        free(distance_vertex);
        if (cur_vertex == goal_vertex)
        {
            goal_vertex_found = true;
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
            distance_neighbor[0] = (void*)distance;
            distance_neighbor[1] = neighbor;
            heap_push(to_visit, distance_neighbor);
        }
        if (goal_vertex_found)
        {
            break;
        }
    }

    for (uint64_t i = 0; i < to_visit->count; i++)
    {
        free(heap_pop(to_visit));
    }

    struct list* path_list = NULL;
    if (!goal_vertex_found)
    {
        fprintf(stderr, "graph_lightest_path: path not found.\n");
    }
    else
    {
        path_list = path_from_parent_map(parents, start_vertex, goal_vertex);
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

    void** distance_start_vertex_null = malloc(sizeof(void*) * 3);
    void* start_vertex = hashset_get_random(graph->vertices);
    distance_start_vertex_null[0] = (uint64_t*)0;
    distance_start_vertex_null[1] = start_vertex;
    distance_start_vertex_null[2] = NULL;

    heap_push(to_visit, distance_start_vertex_null);

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
        if (cur_vertex != start_vertex)
        {
            graph_add_weighted_edge(spanning_tree, parent, cur_vertex, weight);
        }

        struct list* cur_vertex_neighbors = graph_get_vertex_edges(graph, cur_vertex);

        for (uint64_t i = 0; i < cur_vertex_neighbors->count; i++)
        {
            struct vertex_weight* neighbor_weight = list_get(cur_vertex_neighbors, i);
            if (neighbor_weight->vertex == start_vertex)
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

struct graph* graph_spanning_tree(const struct graph* graph)
{
    struct hashset* visited = hashset_create(32, false);
    struct stack* to_visit = stack_create(32);
    struct graph* spanning_tree = graph_create(true);
    void* start_vertex = hashset_get_random(graph->vertices);

    stack_push(to_visit, start_vertex);
    while (to_visit->count > 0)
    {
        void* cur_vertex = stack_pop(to_visit);

        struct list* cur_vertex_neighbors = graph_get_vertex_edges(graph, cur_vertex);
        for (uint64_t i = 0; i < cur_vertex_neighbors->count; i++)
        {
            struct vertex_weight* neighbor_weight = list_get(cur_vertex_neighbors, i);
            void* neighbor = neighbor_weight->vertex;

            if (!hashset_contains(visited, neighbor))
            {
                stack_push(to_visit, neighbor);
                hashset_put(visited, neighbor);
                if (!graph_has_edge(spanning_tree, neighbor, cur_vertex))
                {
                    graph_add_edge(spanning_tree, cur_vertex, neighbor);
                }
            }
        }
    }
    return spanning_tree;
}

struct list* graph_all_reachable_from(const struct graph* graph, void* start_vertex)
{
    struct list* reachable_vertices = list_create(32);
    if (!hashset_contains(graph->vertices, start_vertex))
    {
        fprintf(stderr, "graph_all_reachable_from: start_vertex vertex is not in graph.");
        return reachable_vertices;
    }
    struct hashset* visited = hashset_create(32, false);
    struct stack* to_visit = stack_create(32);

    stack_push(to_visit, start_vertex);
    while (to_visit->count > 0)
    {
        void* cur_vertex = stack_pop(to_visit);
        list_append(reachable_vertices, cur_vertex);

        struct list* cur_vertex_neighbors = graph_get_vertex_edges(graph, cur_vertex);
        for (uint64_t i = 0; i < cur_vertex_neighbors->count; i++)
        {
            struct vertex_weight* neighbor_weight = list_get(cur_vertex_neighbors, i);
            void* neighbor = neighbor_weight->vertex;
            if (!hashset_contains(visited, neighbor))
            {
                stack_push(to_visit, neighbor);
                hashset_put(visited, neighbor);
            }
        }
    }
    hashset_destroy(visited);
    stack_destroy(to_visit);
    return reachable_vertices;
}

static void separation_vertices_recursive(const struct graph* graph, void* parent, const uint64_t cur_depth, struct hashmap* vertex_depths, struct hashmap* vertex_lows, struct hashmap* vertex_parents, struct list* separation_vertices)
{
    hashmap_put(vertex_depths, parent, (uint64_t*)cur_depth);
    hashmap_put(vertex_lows, parent, (uint64_t*)cur_depth);

    struct list* parent_child_weights = graph_get_vertex_edges(graph, parent);
    for (uint64_t i = 0; i < parent_child_weights->count; i++)
    {
        struct vertex_weight* child_weight = list_get(parent_child_weights, i);
        void* child = child_weight->vertex;
        if (hashmap_has_key(vertex_parents, child) && hashmap_get(vertex_parents, child) == parent)
        {
            continue;
        }
        if (hashmap_has_key(vertex_depths, child))
        {
            uint64_t child_depth = (uint64_t)hashmap_get(vertex_depths, child);
            uint64_t parent_low = (uint64_t)hashmap_get(vertex_lows, parent);
            uint64_t depth_low_min = child_depth < parent_low ? child_depth : parent_low;
            hashmap_put(vertex_lows, parent, (uint64_t*)depth_low_min);
            continue;
        }

        hashmap_put(vertex_parents, child, parent);
        separation_vertices_recursive(graph, child, cur_depth + 1, vertex_depths, vertex_lows, vertex_parents, separation_vertices);

        uint64_t child_low = (uint64_t)hashmap_get(vertex_lows, child);
        uint64_t parent_low = (uint64_t)hashmap_get(vertex_lows, parent);
        uint64_t low_min = child_low < parent_low ? child_low : parent_low;
        hashmap_put(vertex_lows, parent, (uint64_t*)low_min);

        if (cur_depth <= (uint64_t)hashmap_get(vertex_lows, child))
        {
            list_append(separation_vertices, parent);
        }
    }
}

struct list* graph_separation_vertices(const struct graph* graph)
{
    struct list* separation_vertices = list_create(32);
    struct hashmap* vertex_depths = hashmap_create(32, false);
    struct hashmap* vertex_lows = hashmap_create(32, false);
    struct hashmap* vertex_parents = hashmap_create(32, false);

    void* start_vertex = hashset_get_random(graph->vertices);
    hashmap_put(vertex_depths, start_vertex, (uint64_t*)0);
    hashmap_put(vertex_lows, start_vertex, (uint64_t*)0);

    struct list* start_child_weights = graph_get_vertex_edges(graph, start_vertex);
    for (uint64_t i = 0; i < start_child_weights->count; i++)
    {
        struct vertex_weight* child_weight = list_get(start_child_weights, i);
        void* child = child_weight->vertex;

        if (!hashmap_has_key(vertex_depths, child))
        {
            hashmap_put(vertex_parents, child, start_vertex);
            separation_vertices_recursive(graph, child, 1, vertex_depths, vertex_lows, vertex_parents, separation_vertices);
        }
    }

    uint64_t unvisited_start_neighbors = 0;
    for (uint64_t i = 0; i < start_child_weights->count; i++)
    {
        struct vertex_weight* neighbor_weight = list_get(start_child_weights, i);
        void* neighbor = neighbor_weight->vertex;

        if ((uint64_t)hashmap_get(vertex_depths, neighbor) == 1)
        {
            unvisited_start_neighbors++;
        }
        if (unvisited_start_neighbors > 1)
        {
            break;
        }
    }
    if (unvisited_start_neighbors > 1)
    {
        list_append(separation_vertices, start_vertex);
    }

    hashmap_destroy(vertex_depths);
    hashmap_destroy(vertex_lows);
    hashmap_destroy(vertex_parents);

    return separation_vertices;
}

static void topological_sort_recursive(const struct graph* graph, void* cur_vertex, struct hashset* visited, struct stack* sorted_vertices)
{
    hashset_put(visited, cur_vertex);

    struct list* cur_vertex_neighbors = graph_get_vertex_edges(graph, cur_vertex);
    for (uint64_t i = 0; i < cur_vertex_neighbors->count; i++)
    {
        struct vertex_weight* neighbor_weight = list_get(cur_vertex_neighbors, i);
        void* neighbor = neighbor_weight->vertex;

        if (!hashset_contains(visited, neighbor))
        {
            topological_sort_recursive(graph, neighbor, visited, sorted_vertices);
        }
    }
    stack_push(sorted_vertices, cur_vertex);
}

struct stack* graph_topological_sort(const struct graph* graph)
{
    struct stack* sorted_vertices = stack_create(32);
    struct hashset* visited = hashset_create(32, false);

    for (uint64_t i = 0; i < graph->num_vertices; i++)
    {
        void* cur_vertex = list_get(graph->vertices_list, i);
        if (!hashset_contains(visited, cur_vertex)) 
        {
            topological_sort_recursive(graph, cur_vertex, visited, sorted_vertices);
        }
    }
    hashset_destroy(visited);
    return sorted_vertices;
}

static void strongly_connected_components_recursive(const struct graph* graph, void* cur_vertex, struct hashset* visited, struct list* component)
{
    hashset_put(visited, cur_vertex);

    struct list* cur_vertex_neighbors = graph_get_vertex_edges(graph, cur_vertex);
    for (uint64_t i = 0; i < cur_vertex_neighbors->count; i++)
    {
        struct vertex_weight* neighbor_weight = list_get(cur_vertex_neighbors, i);
        void* neighbor = neighbor_weight->vertex;

        if (!hashset_contains(visited, neighbor))
        {
            strongly_connected_components_recursive(graph, neighbor, visited, component);
        }
    }
    list_append(component, cur_vertex);
}

struct list* graph_strongly_connected_components(const struct graph* graph)
{
    struct list* components = list_create(32);
    struct hashset* visited = hashset_create(32, false);

    struct stack* topological_sort = graph_topological_sort(graph);
    struct graph* reversed_graph = graph_get_reverse(graph);
    for (uint64_t i = 0; i < graph->num_vertices; i++)
    {
        void* cur_vertex = stack_pop(topological_sort);
        if (!hashset_contains(visited, cur_vertex))
        {
            struct list* component = list_create(32);
            strongly_connected_components_recursive(reversed_graph, cur_vertex, visited, component);
            list_append(components, component);
        }
    }

    hashset_destroy(visited);
    stack_destroy(topological_sort);
    graph_destroy(reversed_graph);
    return components;
}


bool graph_is_biconnected(const struct graph* graph)
{
    struct list* separation_vertices = graph_separation_vertices(graph);
    bool is_biconnected = separation_vertices->count <= 0;
    list_destroy(separation_vertices);
    return is_biconnected;
}


bool is_cyclical_recursive(const struct graph* graph, void* cur_vertex, void* parent_vertex, struct hashset* visited, struct hashset* cur_recursion_stack)
{
    hashset_put(visited, cur_vertex);
    hashset_put(cur_recursion_stack, cur_vertex);

    struct list* cur_vertex_neighbors = graph_get_vertex_edges(graph, cur_vertex);
    for (uint64_t i = 0; i < cur_vertex_neighbors->count; i++)
    {
        struct vertex_weight* neighbor_weight = list_get(cur_vertex_neighbors, i);
        void* neighbor = neighbor_weight->vertex;

        if (!hashset_contains(visited, neighbor) && is_cyclical_recursive(graph, neighbor, cur_vertex, visited, cur_recursion_stack))
        {
            return true;
        }
        else if (hashset_contains(cur_recursion_stack, neighbor) && !(!graph->is_directed && neighbor == parent_vertex))
        {
            return true;
        }
    }
    hashset_remove(cur_recursion_stack, cur_vertex);
    return false;
}

bool graph_is_cyclical(const struct graph* graph)
{
    struct hashset* visited = hashset_create(32, false);
    struct hashset* cur_recursion_stack = hashset_create(32, false);
    bool is_cyclical = false;

    for (uint64_t i = 0; i < graph->num_vertices; i++)
    {
        void* cur_vertex = list_get(graph->vertices_list, i);
        if (!hashset_contains(visited, cur_vertex) && is_cyclical_recursive(graph, cur_vertex, NULL, visited, cur_recursion_stack))
        {
            is_cyclical = true;
            break;
        }
    }

    hashset_destroy(visited);
    hashset_destroy(cur_recursion_stack);
    return is_cyclical;
}


struct list* graph_get_vertex_edges(const struct graph* graph, void* vertex)
{
    return hashmap_get(graph->edges, vertex);
}

struct vertex_weight* graph_get_vertex_edge(const struct graph* graph, void* from, void* to)
{
    struct list* vertex_weights = hashmap_get(graph->edges, from);
    for (uint64_t i = 0; i < vertex_weights->count; i++)
    {
        struct vertex_weight* vertex_weight = list_get(vertex_weights, i);
        if (vertex_weight->vertex == to)
        {
            return vertex_weight;
        }
    }
    fprintf(stderr, "graph_get_edge: edge does not exist.");
    return NULL;
}


struct graph* graph_get_reverse(const struct graph* graph)
{
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
    return reversed_graph;
}

struct graph* graph_reverse_direction(struct graph* graph)
{
    struct graph* reversed_graph = graph_get_reverse(graph);
    *graph = *reversed_graph;
    free(reversed_graph);
    return graph;
}


void graph_print_vertex_neighbors_char(const struct graph* graph, void* vertex)
{
    struct list* vertex_neigbor_weights = graph_get_vertex_edges(graph, vertex);
    printf("(");
    for (uint64_t i = 0; i < vertex_neigbor_weights->count; i++)
    {
        if (list_get(vertex_neigbor_weights, i) == NULL)
        {
            continue;
        }
        char neighbor_char = (char)(int64_t)((struct vertex_weight*)list_get(vertex_neigbor_weights, i))->vertex;
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
        if (list_get(vertex_neigbor_weights, i) == NULL)
        {
            continue;
        }
        int64_t neighbor_int = (int64_t)((struct vertex_weight*)list_get(vertex_neigbor_weights, i))->vertex;
        printf(i == vertex_neigbor_weights->count - 1 ? "%lld" : "%lld, ", neighbor_int);
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

