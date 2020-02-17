#include "tree.h"

void tree_insert(tree* tree, node* key) //if already exist do nothing
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
        else if (key->data_ > cur->data_)
        {
            cur = cur->right_;
        }
        else
        {
            return;
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

void tree_delete(tree* tree, node* key) //if no exist does nothing
{
    node* cur = tree->root_;
    node** prev_pos = nullptr;
    while (cur->data_ != key->data_ && cur != nullptr)
    {
        if (key->data_ < cur->data_)
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

    if (cur == nullptr)
    {
        return;
    }

    if (cur->left_ == nullptr && cur->right_ == nullptr)
    {
        *prev_pos = nullptr;
        free(cur);
    }
    else if (cur->left_ != nullptr && cur->right_ == nullptr) //optimize
    {
        *prev_pos = cur->left_;
        free(cur);
    }
    else if (cur->left_ == nullptr && cur->right_ != nullptr) //optimize
    {
        *prev_pos = cur->right_;
        free(cur);
    }
    else
    {
        node* next = seach_next(tree, key);
        if (next == cur->right_)
        {
            prev_pos = next;
            free(cur);
        }
        else
        {
            next->prev_->left_ = next->right_; //???!!! maybe wrong
            prev_pos = next;
            free(cur);
        }
    }
}

node* tree_search(tree* tree, int key) //if no exist returns nullptr
{
    node* cur = tree->root_;
    while (cur->data_ != key->data_ && cur != nullptr)
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

node* seach_next(tree* tree, int key) //if no key returns nullptr
{
    node* cur = tree_search(tree, key);
    if (cur == nullptr)
    {
        return nullptr;
    }

    if (cur->right_ != nullptr)
    {
        return tree_min(cur->right_)
    }

    node* prev = cur->prev_;
    while (prev != nullptr && cur == prev.right_)
    {
        cur = prev;
        prev = prev.prev_;
    }

    return prev;
}

node* tree_min(node* root)
{
    node* cur = root;
    while (cur->left_ != nullptr)
    {
        cur = cur->left_;
    }

    return cur;
}

node* tree_max(node* root)
{
    node* cur = root;
    while (cur->right_ != nullptr)
    {
        cur = cur->right_;
    }

    return cur;
}

void print(node* root)
{
    node* cur = root;
    if (cur != nullptr)
    {
        print(cur->left_);
        printf("%d ", cur->data_);
        print(cur->right_);
    }
}