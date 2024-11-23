#ifndef LIST_SORT_H
#define LIST_SORT_H

#include "list.h"
#include "hashmap.h"

struct list* list_sort_bubble(struct list* list);
struct list* list_sort_selection(struct list* list);
struct list* list_sort_insertion(struct list* list);
struct list* list_sort_heap(struct list* list);
struct list* list_sort_merge(struct list* list);
struct list* list_sort_quick(struct list* list);
struct list* list_sort_radix(struct list* list);


#endif
