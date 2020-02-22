#include "tree.h"

struct node
{
    struct node* prev_;
    struct node* left_;
    struct node* right_;
    int data_;
};

struct tree
{
    struct node* root_;
};

tree* new_tree()
{
    tree* new = (tree*)calloc(1, sizeof(tree));
    new->root_ = NULL;
    return new;
}

int tree_insert(tree* tree, int key_)
{
    node* key = (node*)calloc(1, sizeof(node));
    key->data_ = key_;

    node* prev = NULL;
    node* cur = tree->root_;
    while (cur != NULL)
    {
        prev = cur;
        if (key->data_ < cur->data_)
        {
            cur = cur->left_;
        }
        else if (key->data_ > cur->data_)
        {
            cur = cur->right_;
        }
        else
        {
            return -1;
        }
    }

    key->prev_ = prev;
    if (prev == NULL)
    {
        tree->root_ = key;
    }
    else if (key->data_ < prev->data_)
    {
        prev->left_ = key;
    }
    else
    {
        prev->right_ = key;
    }

    return 0;
}

int tree_delete(tree* tree, int key)
{
    node* cur = tree->root_;
    node** prev_pos = NULL;
    while (cur != NULL && cur->data_ != key)
    {
        if (key < cur->data_)
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
        free(cur);
    }
    else if (cur->left_ != NULL && cur->right_ == NULL)
    {
        *prev_pos = cur->left_;
        free(cur);
    }
    else if (cur->left_ == NULL && cur->right_ != NULL)
    {
        *prev_pos = cur->right_;
        free(cur);
    }
    else
    {
        node* next = search_next(tree, key);
        if (next == cur->right_)
        {
            *prev_pos = next;
            next->left_ = cur->left_;
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
            free(cur);
        }
    }

    return 0;
}

node* tree_search(tree* tree, int key)
{
    node* cur = tree->root_;
    while (cur != NULL && cur->data_ != key)
    {
        if (cur == NULL)
        {
            return NULL;
        }

        if (key < cur->data_)
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

node* search_next(tree* tree, int key)
{
    node* cur = tree_search(tree, key);
    if (cur == NULL)
    {
        return NULL;
    }

    if (cur->right_ != NULL)
    {
        return tree_min_private(cur->right_);
    }

    node* prev = cur->prev_;
    while (prev != NULL && cur == prev->right_)
    {
        cur = prev;
        prev = prev->prev_;
    }

    return prev;
}

node* tree_min(tree* tree)
{
    return tree_min_private(tree->root_);
}

node* tree_min_private(node* root)
{
    if (root == NULL)
    {
        return NULL;
    }

    node* cur = root;
    while (cur->left_ != NULL)
    {
        cur = cur->left_;
    }

    return cur;
}

node* tree_max(tree* tree)
{
    return tree_max_private(tree->root_);
}

node* tree_max_private(node* root)
{
    if (root == NULL)
    {
        return NULL;
    }

    node* cur = root;
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

void print_private(node* root)
{
    node* cur = root;
    if (cur != NULL)
    {
        print_private(cur->left_);
        printf("%d ", cur->data_);
        print_private(cur->right_);
    }
}

void tree_destroy(tree* tree)
{
    tree_destroy_private(tree->root_);
}

void tree_destroy_private(node* root)
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