#include "tree.h"

void tree_insert(tree* tree, node* key)
{
    node* prev = nullptr;
    node* cur = tree->root_;
    while (cur != nullptr)
    {
        prev = cur;
        if (key->data_ < cur->data_)
        {
            cur = cur->left_;
        }
        else
        {
            cur = cur->right_;
        }
    }

    key->prev_ = prev;
    if (prev == nullptr)
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
}

void tree_delete(tree* tree, node* key)
{
    node* cur = tree->root_;
    node** prev_pos = nullptr;
    while (cur->data_ != key->data_)
    {
        if (key->data_ < cur->data_)
        {
            pos = &(cur->left_);
            cur = cur->left_;
        }
        else
        {
            pos = &(cur->right_);
            cur = cur->right_;
        }
    }

    if (cur->left_ == nullptr && cur->right_ == nullptr)
    {
        *prev_pos = nullptr;
        free(cur);
    }
    else if (cur->left_ != nullptr && cur->right_ == nullptr)
    {
        *prev_pos = cur->left_;
        free(cur);
    }
    else if (cur->left_ == nullptr && cur->right_ != nullptr)
    {
        *prev_pos = cur->right_;
        free(cur);
    }
    else
    {
        
    }
}

node* tree_search(int key)
{
    node* cur = tree->root_;
    while (cur->data_ != key->data_)
    {
        if (key->data_ < cur->data_)
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