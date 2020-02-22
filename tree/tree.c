#include "tree.h"

struct private_node
{
    private_node* prev_;
    private_node* left_;
    private_node* right_;
    node* data_;
    int key_;
};

struct tree
{
    struct private_node* root_;
};

tree* new_tree()
{
    tree* new = (tree*)calloc(1, sizeof(tree));
    new->root_ = NULL;
    return new;
}

int tree_insert(tree* tree, int key, node* data_node)
{
    private_node* new = (private_node*)calloc(1, sizeof(private_node));
    node* tmp = (node*)calloc(1, sizeof(node));
    tmp->data_ = data_node->data_;
    new->key_ = key;
    new->data_ = tmp;

    private_node* prev = NULL;
    private_node* cur = tree->root_;
    while (cur != NULL)
    {
        prev = cur;
        if (new->key_ < cur->key_)
        {
            cur = cur->left_;
        }
        else if (new->key_ > cur->key_)
        {
            cur = cur->right_;
        }
        else
        {
            return -1;
        }
    }

    new->prev_ = prev;
    if (prev == NULL)
    {
        tree->root_ = new;
    }
    else if (new->key_ < prev->key_)
    {
        prev->left_ = new;
    }
    else
    {
        prev->right_ = new;
    }

    return 0;
}

int tree_delete(tree* tree, int key)
{
    private_node* cur = tree->root_;
    private_node** prev_pos = NULL;
    while (cur != NULL && cur->key_ != key)
    {
        if (key < cur->key_)
        {
            prev_pos = &(cur->left_);
            cur = cur->left_;
        }
        else
        {
            prev_pos = &(cur->right_);
            cur = cur->right_;
        }
    }

    if (cur == NULL)
    {
        return -1;
    }

    if (cur->left_ == NULL && cur->right_ == NULL)
    {
        *prev_pos = NULL;
        free(cur->data_);
        free(cur);
    }
    else if (cur->left_ != NULL && cur->right_ == NULL)
    {
        *prev_pos = cur->left_;
        free(cur->data_);
        free(cur);
    }
    else if (cur->left_ == NULL && cur->right_ != NULL)
    {
        *prev_pos = cur->right_;
        free(cur->data_);
        free(cur);
    }
    else
    {
        private_node* next = search_next_private(tree, key);
        if (next == cur->right_)
        {
            *prev_pos = next;
            next->left_ = cur->left_;
            free(cur->data_);
            free(cur);
        }
        else
        {
            next->prev_->left_ = next->right_;
            if (prev_pos != NULL)
            {
                *prev_pos = next;
            }
            else
            {
                tree->root_ = next;
            }

            next->left_ = cur->left_;
            next->right_ = cur->right_;
            free(cur->data_);
            free(cur);
        }
    }

    return 0;
}

node* tree_search(tree* tree, int key)
{
    private_node* result = tree_search_private(tree, key);
    if (result == NULL)
    {
        return NULL;
    }

    return result->data_;
}

private_node* tree_search_private(tree* tree, int key)
{
    private_node* cur = tree->root_;
    while (cur != NULL && cur->key_ != key)
    {
        if (cur == NULL)
        {
            return NULL;
        }

        if (key < cur->key_)
        {
            cur = cur->left_;
        }
        else
        {
            cur = cur->right_;
        }
    }

    return cur;
}

int search_next(tree* tree, int key)
{
    private_node* result = search_next_private(tree, key);
    if (result == NULL)
    {
        errno = EINVAL;
        return -1;
    }

    return result->key_;
}

private_node* search_next_private(tree* tree, int key)
{
    private_node* cur = tree_search_private(tree, key);
    if (cur == NULL)
    {
        return NULL;
    }

    if (cur->right_ != NULL)
    {
        return tree_min_private(cur->right_);
    }

    private_node* prev = cur->prev_;
    while (prev != NULL && cur == prev->right_)
    {
        cur = prev;
        prev = prev->prev_;
    }

    return prev;
}

int tree_min(tree* tree)
{
    private_node* result = tree_min_private(tree->root_);
    if (result == NULL)
    {
        errno = EINVAL;
        return -1;
    }

    return result->key_;
}

private_node* tree_min_private(private_node* root)
{
    if (root == NULL)
    {
        return NULL;
    }

    private_node* cur = root;
    while (cur->left_ != NULL)
    {
        cur = cur->left_;
    }

    return cur;
}

int tree_max(tree* tree)
{
    private_node* result = tree_max_private(tree->root_);
    if (result == NULL)
    {
        errno = EINVAL;
        return -1;
    }

    return result->key_;
}

private_node* tree_max_private(private_node* root)
{
    if (root == NULL)
    {
        return NULL;
    }

    private_node* cur = root;
    while (cur->right_ != NULL)
    {
        cur = cur->right_;
    }

    return cur;
}

void print(tree* tree)
{
    print_private(tree->root_);
}

void print_private(private_node* root)
{
    private_node* cur = root;
    if (cur != NULL)
    {
        print_private(cur->left_);
        printf("{%d, %c} ", cur->key_, cur->data_->data_);
        print_private(cur->right_);
    }
}

void tree_destroy(tree* tree)
{
    tree_destroy_private(tree->root_);
}

void tree_destroy_private(private_node* root)
{
    if (root->left_ != NULL)
    {
        tree_destroy_private(root->left_);
        root->left_ = NULL;
    }

    if (root->right_ != NULL)
    {
        tree_destroy_private(root-> right_);
        root->right_ = NULL;
    }

    if ((root->right_ = NULL) && (root->left_ = NULL))
    {
        free(root);
    }
}