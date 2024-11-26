#include "bst.h"


struct bst* bst_create(const bool is_avl)
{
    struct bst* bst = malloc(sizeof(struct bst));
    bst->is_avl = is_avl;
    bst->root = NULL;
    return bst;
}

static struct node* node_create(const int64_t data)
{
    struct node* node = malloc(sizeof(struct node));
    node->data = data;
    node->left = NULL;
    node->right = NULL;
    node->height = 0;
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

static struct node* rotate_left(struct node** node)
{
    struct node* right = (*node)->right;
    struct node* right_left = right->left;
    right->left = *node;
    (*node)->right = right_left;
    return right;
}

static struct node* rotate_right(struct node** node)
{
    struct node* left = (*node)->left;
    struct node* left_right = left->right;
    left->right = *node;
    (*node)->left = left_right;
    return left;
}

static int16_t get_height(const struct node* node)
{
    if (node == NULL)
    {
        return -1;
    }
    return node->height;
}

static void set_height(struct node* node)
{
    if (node == NULL)
    {
        return;
    }
    set_height(node->left);
    set_height(node->right);

    int16_t left_height = get_height(node->left);
    int16_t right_height = get_height(node->right);

    node->height = 1 + (left_height >= right_height ? left_height : right_height);
}

static int16_t balance_factor(const struct node* node)
{
    if (node == NULL)
    {
        return 0;
    }
    return get_height(node->left) - get_height(node->right);
}

static struct node* balance(struct node** node)
{
    if (balance_factor(*node) < -1)
    {
        if (balance_factor((*node)->right) > 0)
        {
            (*node)->right = rotate_right(&(*node)->right);
        }
        return rotate_left(node);
    }
    if (balance_factor(*node) > 1)
    {
        if (balance_factor((*node)->left) < 0)
        {
            (*node)->left = rotate_left(&(*node)->left);
        }
        return rotate_right(node);
    }
    return *node;
}

static void insert_recursive(struct node** node, const int64_t data, const bool is_avl)
{
    if (*node == NULL)
    {
        *node = node_create(data);
    }
    else if (data <= (*node)->data)
    {
        insert_recursive(&(*node)->left, data, is_avl);
    }
    else if (data > (*node)->data)
    {
        insert_recursive(&(*node)->right, data, is_avl);
    }
    if (is_avl)
    {
        set_height(*node);
        *node = balance(node);
    }
}

void bst_insert(struct bst* bst, const int64_t data)
{
    insert_recursive(&bst->root, data, bst->is_avl);
}

static struct node* get_min_child(struct node* node)
{
    if (node->left == NULL)
    {
        return node;
    }
    return get_min_child(node);
}

struct node* remove_recursive(struct node** node, const int64_t data, const bool is_avl)
{
    if (*node == NULL)
    {
        return NULL;
    }

    else if (data < (*node)->data)
    {
        (*node)->left = remove_recursive(&(*node)->left, data, is_avl);
    }
    else if (data > (*node)->data)
    {
        (*node)->right = remove_recursive(&(*node)->right, data, is_avl);
    }

    else if ((*node)->left == NULL)
    {
        *node = (*node)->right;
    }
    else if ((*node)->right == NULL)
    {
        *node = (*node)->left;
    }
    else
    {
        struct node* right_min_child = get_min_child((*node)->right);
        (*node)->data = right_min_child->data;
        (*node)->right = remove_recursive(&(*node)->right, right_min_child->data, is_avl);
    }
    
    if (is_avl)
    {
        set_height(*node);
        return balance(node);
    }
    return *node;
}

void bst_remove(struct bst* bst, int64_t data)
{
    remove_recursive(&bst->root, data, bst->is_avl);
}


int64_t get_recursive(const struct node* node, const int64_t data)
{
    if (node == NULL)
    {
        fprintf(stderr, "bst_get: data not in tree.");
        return 0;
    }
    if (data == node->data)
    {
        return node->data;
    }
    if (data < node->data)
    {
        return get_recursive(node->left, data);
    }
    if (data > node->data)
    {
        return get_recursive(node->right, data);
    }
    return 0;
}

int64_t bst_get(const struct bst* bst, const int64_t data)
{
    return get_recursive(bst->root, data);
}

int64_t contains_recursive(const struct node* node, const int64_t data)
{
    if (node == NULL)
    {
        return false;
    }
    if (data == node->data)
    {
        return true;
    }
    if (data < node->data)
    {
        return contains_recursive(node->left, data);
    }
    if (data > node->data)
    {
        return contains_recursive(node->right, data);
    }
    return false;
}

bool bst_contains(const struct bst* bst, const int64_t data)
{
    return contains_recursive(bst->root, data);
}


void get_range_recursive(const struct node* node, const int64_t from, const int64_t to, struct list* list)
{
    if (node == NULL)
    {
        return;
    }
    if (node->data >= from)
    {
        get_range_recursive(node->left, from, to, list);
    }
    if (node->data >= from && node->data <= to)
    {
        list_append(list, (void*)node->data);
    }
    if (node->data <= to)
    {
        get_range_recursive(node->right, from, to, list);
    }
}

struct list* bst_get_range(const struct bst* bst, const int64_t from, const int64_t to)
{
    struct list* list = list_create(32);
    get_range_recursive(bst->root, from, to, list);
    return list;
}

int64_t bst_get_min(const struct bst* bst)
{
    return get_min_child(bst->root)->data;
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
