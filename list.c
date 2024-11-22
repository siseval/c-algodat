#include "list.h"


static bool list_realloc(struct list* list, uint64_t size)
{
    void** tmp = realloc(list->data, size); 
    if (!tmp)
    {
        fprintf(stderr, "list_realloc: realloc failed\n");
        free(list->data);
        free(list);
        return false;
    }
    list->data = tmp;
    return true;
}

struct list* list_create(const uint64_t size)
{
    struct list* list = calloc(1, sizeof(struct list));
    if (!list)
    {
        fprintf(stderr, "list_create: list malloc failed\n");
        return NULL;
    }

    list->size = size;
    list->data_size = sizeof(void*);
    list->count = 0;
    list->data = calloc((1 + size), list->data_size);

    if (!list->data) 
    {
        fprintf(stderr, "list_create: data malloc failed\n");
        free(list);
        return NULL;
    }

    return list;
}

struct list* list_create_copy(const struct list* list)
{
    struct list* list_copy = list_create(32);
    for (uint64_t i = 0; i < list->count; i++)
    {
        list_append(list_copy, list_get(list, i));
    }
    return list_copy;
}

void list_destroy(struct list* list)
{
    free(list->data);
    free(list);
}

void list_append(struct list* list, void* data)
{
    if (list->count >= list->size)
    {
        if (!list_realloc(list, list->data_size + (list->size * 2) * list->data_size))
        {
            return; 
        }
        list->size *= 2;
    }
    list->data[1 + list->count] = data;
    list->count++;
}

void list_insert(struct list* list, void* data, const uint64_t index)
{
    if (index >= list->count)
    {
        fprintf(stderr, "list_index: index out of bounds\n");
        return;
    }
 
    if (list->count >= list->size)
    {
        if (!list_realloc(list, list->data_size + (list->size * 2) * list->data_size))
        {
            return; 
        }
        list->size *= 2;
    }

    for (int i = 1 + list->count; i > 1 + index; i--)
    {
        list->data[i] = list->data[i - 1];    
    }
    list->data[1 + index] = data;
    list->count++;
}

void list_replace(struct list* list, void* data, const uint64_t index)
{
        if (index >= list->count)
        {
            fprintf(stderr, "list_replace: index out of bounds\n");
            return;
        }

    list->data[1 + index] = data;
}


void* list_remove_at(struct list* list, const uint64_t index)
{
    if (index >= list->count)
    {
        fprintf(stderr, "list_remove: index out of bounds\n");
        return NULL;
    }
    void* data_buf = list->data[1 + index];

    for (int i = index + 2; i <= list->count; i++)
    {
        list->data[(i - 1)] = list->data[i];
    }

    list->data[list->count] = NULL;
    list->count--;

    return data_buf;
}

void* list_remove(struct list* list, void* data)
{
    for (uint64_t i = 0; i < list->count; i++)
    {
        if (list_get(list, i) == data)
        {
            return list_remove_at(list, i);
        }
    }
    fprintf(stderr, "list_remove: data not in list.\n");
    return NULL;
}

void list_clear(struct list* list)
{
    list->count = 0;
}


struct list* list_reverse(struct list* list)
{
    uint64_t left_index = 0;
    uint64_t right_index = list->count - 1;

    while (left_index < right_index)
    {
        void* left_data = list_get(list, left_index);
        void* right_data = list_get(list, right_index);
        list_replace(list, left_data, right_index);
        list_replace(list, right_data, left_index);
        left_index++;
        right_index--;
    }
    return list;
}

struct list *list_swap(struct list* list, const uint64_t index_a, const uint64_t index_b)
{
    if (index_a < 0 || index_a >= list->count || index_b < 0 || index_b >= list->count)
    {
        fprintf(stderr, "list_swap: index out of bounds: a: %lld, b: %lld\n", index_a, index_b);
        return NULL;
    }
    void* data_buf = list->data[1 + index_a];
    list->data[1 + index_a] = list->data[1 + index_b];
    list->data[1 + index_b] = data_buf;
    return list;
}


struct list* list_get_sublist(const struct list* list, const uint64_t from_index, const uint64_t to_index)
{
    struct list* sublist = list_create(32);
    for (uint64_t i = from_index; i < to_index; i++)
    {
        list_append(sublist, list_get(list, i));
    }
    return sublist;
}


void* list_get(const struct list* list, const uint64_t index)
{
    if (index >= list->count)
    {
        fprintf(stderr, "list_get: index out of bounds: %lld\n", index);
        return NULL;
    }
    return list->data[1 + index];
}

bool list_contains(const struct list* list, const void* data)
{
    for (int i = 0; i < list->count; i++)
    {
        if (list->data[1 + i] == data)
        {
            return true;
        }
    }
    return false;
}


bool list_is_sorted(struct list* list)
{
    for (uint64_t i = 0; i < list->count - 1; i++)
    {
        if ((uint64_t)list_get(list, i) > (uint64_t)list_get(list, i + 1))
        {
            return false;
        }
    }
    return true;
}


struct list* list_bubble_sort(struct list* list)
{
    for (uint64_t i = 0; i < list->count - 1; i++)
    {
        bool is_sorted = true;
        for (uint64_t j = 0; j < list->count - i - 1; j++)
        {
            if (list_get(list, j) > list_get(list, j + 1))
            {
                is_sorted = false;
                list_swap(list, j, j + 1);
            }
        }
        if (is_sorted)
        {
            break;
        }
    }
    return list;
}

struct list* list_selection_sort(struct list* list)
{
    for (uint64_t i = 0; i < list->count; i++)
    {
        void* min = (void*)INT_MAX;
        uint64_t min_index = 0;

        for (uint64_t j = i; j < list->count; j++)
        {
            void* cur_data = list_get(list, j);
            if (cur_data < min)
            {
                min = cur_data;
                min_index = j;
            }
        }
        list_swap(list, i, min_index);
    }
    return list;
}

struct list* list_insertion_sort(struct list* list)
{
    for (uint64_t i = 1; i < list->count; i++)
    {
        for (uint64_t j = i; j > 0; j--)
        {
            if (list_get(list, j - 1) > list_get(list, j))
            {
                list_swap(list, j - 1, j);
            }
        }
    }
    return list;
}

struct list* list_heap_sort(struct list* list)
{
    for (uint64_t i = 1; i < list->count; i++)
    {
        uint64_t index = i;
        uint64_t parent_index = (index - 1) / 2;

        while (index > 0 && list_get(list, index) > list_get(list, parent_index))
        {
            list_swap(list, index, parent_index);
            index = parent_index;
            parent_index = (index - 1) / 2;
        }
    }
    for (uint64_t i = list->count - 1; i > 0; i--)
    {
        list_swap(list, 0, i);
        uint64_t index = 0;

        while (index * 2 + 1 < i)
        {
            uint64_t left_index = index * 2 + 1;
            uint64_t right_index = index * 2 + 2;
            void* left_data = list_get(list, left_index);
            void* right_data = list_get(list, right_index);
            
            uint64_t candidate_index = right_index >= i || left_data >= right_data ? left_index : right_index;
            if (list_get(list, index) < list_get(list, candidate_index))
            {
                list_swap(list, index, candidate_index);
                index = candidate_index;
            }
            else
            {
                break;
            }
        }
    }
    return list; 
}

static struct list* merge_sort_merge(struct list* sublist_a, struct list* sublist_b)
{
    struct list* sorted_list = list_create(32);
    uint64_t index_a = 0;
    uint64_t index_b = 0;

    while (index_a < sublist_a->count && index_b < sublist_b->count)
    {
        void* data_a = list_get(sublist_a, index_a);
        void* data_b = list_get(sublist_b, index_b);
        if (data_a <= data_b)
        {
            list_append(sorted_list, data_a);
            index_a++;
        }
        else
        {
            list_append(sorted_list, data_b);
            index_b++;
        }
    }
    while (index_a < sublist_a->count)
    {
        list_append(sorted_list, list_get(sublist_a, index_a));
        index_a++;
    }
    while (index_b < sublist_b->count)
    {
        list_append(sorted_list, list_get(sublist_b, index_b));
        index_b++;
    }

    list_destroy(sublist_a);
    list_destroy(sublist_b);
    return sorted_list;
}

static struct list* merge_sort_recursive(struct list* list)
{
    if (list->count <= 1)
    {
        return list;
    }
    uint64_t middle_index = list->count / 2;

    struct list* sublist_a = list_get_sublist(list, 0, middle_index);
    struct list* sublist_b = list_get_sublist(list, middle_index, list->count);
    list_merge_sort(sublist_a);
    list_merge_sort(sublist_b);
    free(list);

    return merge_sort_merge(sublist_a, sublist_b);
}

struct list* list_merge_sort(struct list* list)
{
    struct list* list_copy = list_create_copy(list);
    list_copy = merge_sort_recursive(list_copy);
    *list = *list_copy;
    free(list_copy);
    return list;
}

static uint64_t quick_sort_partition(struct list* list, const uint64_t low_index, const uint64_t high_index)
{
    uint64_t pivot_index = low_index + rand() % (high_index - low_index);
    void* pivot_data = list_get(list, pivot_index);
    list_swap(list, high_index, pivot_index);

    uint64_t left_index = low_index;
    uint64_t right_index = high_index - 1;

    while (left_index <= right_index)
    {
        while (left_index <= right_index && list_get(list, left_index) <= pivot_data)
        {
            left_index++;
        }
        while (right_index >= left_index && list_get(list, right_index) >= pivot_data)
        {
            right_index--;
        }
        if (left_index < right_index)
        {
            list_swap(list, left_index, right_index);
        }
    }

    list_swap(list, left_index, high_index);
    return left_index;
}

static struct list* quick_sort_recursive(struct list* list, const uint64_t low_index, const uint64_t high_index)
{
    if (low_index >= high_index)
    {
        return list;
    }
    uint64_t pivot_index = quick_sort_partition(list, low_index, high_index);

    quick_sort_recursive(list, low_index, pivot_index - 1);
    quick_sort_recursive(list, pivot_index + 1, high_index);

    return list;
}

struct list* list_quick_sort(struct list* list)
{
    return quick_sort_recursive(list, 0, list->count - 1);
}


void list_print_int(const struct list* list)
{
    printf("(");
    for (uint64_t i = 0; i < list->count; i++)
    {
        printf(i == list->count - 1 ? "%lld" : "%lld, ", (uint64_t)list_get(list, i));
    }
    printf(")");
}

void list_print_char(const struct list* list)
{
    printf("(");
    for (uint64_t i = 0; i < list->count; i++)
    {
        printf(i == list->count - 1 ? "%c" : "%c, ", (char)(uint64_t)list_get(list, i));
    }
    printf(")");
}
