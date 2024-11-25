#include "bst.h"


struct bst* bst_create()
{
    struct bst* bst = malloc(sizeof(struct bst));
    bst->count = 0;
    bst->root = NULL;
    return bst;
}

static struct node* node_create(const int64_t data)
{
    struct node* node = malloc(sizeof(struct node));
    node->data = data;
    node->left = NULL;
    node->right = NULL;
    return node;
}

static void node_destroy_recursive(struct node* node)
{
    if (node == NULL)
    {
        return;
    }
    node_destroy_recursive(node->left);
    node_destroy_recursive(node->right);
    free(node);
}

void bst_destroy(struct bst* bst)
{
    node_destroy_recursive(bst->root);
    free(bst);
}

static void insert_recursive(struct node** node, const int64_t data)
{
    if (*node == NULL)
    {
        *node = node_create(data);
        return;
    }
    if (data <= (*node)->data)
    {
        insert_recursive(&(*node)->left, data);
        return;
    }
    if (data > (*node)->data)
    {
        insert_recursive(&(*node)->right, data);
        return;
    }
}

void bst_insert(struct bst* bst, const int64_t data)
{
    insert_recursive(&bst->root, data);
}

void bst_remove(struct bst* bst, int64_t data)
{
}

void bst_replace(struct bst* bst, int64_t vertex, int64_t data)
{
}


int64_t bst_get(const struct bst* bst, int64_t data)
{
    return 0;
}

bool bst_contains(const struct bst* bst, int64_t data)
{
    return false;
}

void get_range_recursive(const struct bst* bst, const int64_t from, const int64_t to, const struct node* node, struct list* list)
{
}

struct list* bst_get_range(const struct bst* bst, const int64_t from, const int64_t to)
{
    struct list* list = list_create(32);
    get_range_recursive(bst, from, to, bst->root, list);
    return list;
}


int64_t bst_get_min(const struct bst* bst)
{
    return 0;
}

static void print_int_recursive(const struct bst* bst, struct node* node, const char* prefix, const bool is_left)
{
    printf("\n%s", prefix);
    char next_prefix[128];
    strcpy(next_prefix, prefix);

    if (node != bst->root)
    {
        printf("%s", is_left ? "├──" : "└──");
        strcat(next_prefix, is_left ? "│  " : "   ");
    }
    if (node == NULL)
    {
        printf("()");
        return;
    }
    printf("%lld", node->data);
    print_int_recursive(bst, node->left, next_prefix, true);
    print_int_recursive(bst, node->right, next_prefix, false);
}

void bst_print_int(const struct bst* bst)
{
    print_int_recursive(bst, bst->root, "", false);
}
