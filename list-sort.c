#include "list-sort.h"

struct list* list_sort_bubble(struct list* list)
{
    for (uint64_t i = 0; i < list->count - 1; i++)
    {
        bool is_sorted = true;
        for (uint64_t j = 0; j < list->count - i - 1; j++)
        {
            if ((int64_t)list_get(list, j) > (int64_t)list_get(list, j + 1))
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

struct list* list_sort_selection(struct list* list)
{
    for (uint64_t i = 0; i < list->count; i++)
    {
        int64_t min = (int64_t)INT_MAX;
        uint64_t min_index = 0;

        for (uint64_t j = i; j < list->count; j++)
        {
            int64_t cur_data = (int64_t)list_get(list, j);
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

struct list* list_sort_insertion(struct list* list)
{
    for (uint64_t i = 1; i < list->count; i++)
    {
        for (uint64_t j = i; j > 0; j--)
        {
            if ((int64_t)list_get(list, j - 1) > (int64_t)list_get(list, j))
            {
                list_swap(list, j - 1, j);
            }
        }
    }
    return list;
}

struct list* list_sort_heap(struct list* list)
{
    for (uint64_t i = 1; i < list->count; i++)
    {
        uint64_t index = i;
        uint64_t parent_index = (index - 1) / 2;

        while (index > 0 && (int64_t)list_get(list, index) > (int64_t)list_get(list, parent_index))
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
            int64_t left_data = (int64_t)list_get(list, left_index);
            int64_t right_data = (int64_t)list_get(list, right_index);
            
            uint64_t candidate_index = right_index >= i || left_data >= right_data ? left_index : right_index;
            if ((int64_t)list_get(list, index) < (int64_t)list_get(list, candidate_index))
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
        int64_t data_a = (int64_t)list_get(sublist_a, index_a);
        int64_t data_b = (int64_t)list_get(sublist_b, index_b);
        if (data_a <= data_b)
        {
            list_append(sorted_list, (void*)data_a);
            index_a++;
        }
        else
        {
            list_append(sorted_list, (void*)data_b);
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
    merge_sort_recursive(sublist_a);
    merge_sort_recursive(sublist_b);

    return merge_sort_merge(sublist_a, sublist_b);
}

struct list* list_sort_merge(struct list* list)
{
    struct list* list_copy = list_create_copy(list);
    list_copy = merge_sort_recursive(list_copy);
    *list = *list_copy;
    free(list_copy);
    return list;
}

static int64_t quick_sort_partition(struct list* list, const int64_t low_index, const int64_t high_index)
{
    int64_t pivot_index = low_index + rand() % (high_index - low_index);
    int64_t pivot_data = (int64_t)list_get(list, pivot_index);
    list_swap(list, pivot_index, high_index);

    int64_t left_index = low_index;
    int64_t right_index = high_index - 1;

    while (left_index <= right_index)
    {
        while (left_index <= right_index && (int64_t)list_get(list, left_index) <= pivot_data)
        {
            left_index++;
        }
        while (right_index >= left_index && (int64_t)list_get(list, right_index) >= pivot_data)
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

static struct list* quick_sort_recursive(struct list* list, const int64_t low_index, const int64_t high_index)
{
    if (low_index >= high_index)
    {
        return list;
    }
    int64_t pivot_index = quick_sort_partition(list, low_index, high_index);

    quick_sort_recursive(list, low_index, pivot_index - 1);
    quick_sort_recursive(list, pivot_index + 1, high_index);

    return list;
}

struct list* list_sort_quick(struct list* list)
{
    return quick_sort_recursive(list, 0, list->count - 1);
}

struct list* bucket_sort_by_digit(struct list* list, const uint64_t exponent)
{
    struct list* buckets = list_create(32);
    for (uint8_t i = 0; i < 20; i++)
    {
        list_append(buckets, list_create(32));
    }

    for (uint64_t i = 0; i < list->count; i++)
    {
        int64_t number = (int64_t)list_get(list, i);
        int16_t sign = number < 0 ? -1 : 1;
        int64_t number_digit = (((sign * number) / exponent) % 10) * sign;
        list_append(list_get(buckets, 10 + number_digit), (void*)number);
    }

    uint64_t index = 0;
    for (uint8_t i = 0; i < 20; i++)
    {
        struct list* bucket = list_get(buckets, i);
        for (uint64_t j = 0; j < bucket->count; j++)
        {
            list_replace(list, list_get(bucket, j), index);
            index++;
        }
        free(bucket);
    }
    free(buckets);

    return list;
}

struct list* list_sort_radix(struct list* list)
{
    uint16_t max_digit_count = 1;
    int64_t list_max = (int64_t)list_get_max(list);

    for (uint64_t i = 1; list_max / i > 0; i *= 10)
    {
        bucket_sort_by_digit(list, i);
    }


    return list;
}
