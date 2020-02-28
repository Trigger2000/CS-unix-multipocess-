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

//Fault injection malloc
void* mymalloc(int size);
//prev - parent node, prev_pos - position relatively to parent node
private_node* tree_search_private(tree* tree, int key, private_node** prev, private_node*** prev_pos);
private_node* tree_min_private(private_node* root);
private_node* tree_max_private(private_node* root);
void tree_destroy_private(private_node* root);
int foreach_private(private_node* root, int(*func)(node* node, void* val), void* par);

void* mymalloc(int size)
{
    static int counter = 0;
    if (counter % 5 == 3)
    {
        counter++;
        return NULL;
    }

    counter++;
    return calloc(1, size);
}

tree* new_tree()
{
    tree* new = (tree*)mymalloc(sizeof(tree));
    if (new == NULL)
    {
        return NULL;
    }
    new->root_ = NULL;
    return new;
}

int tree_insert(tree* tree, int key, const void* data, int data_size)
{
    if (tree == NULL)
        return -1;

    private_node* new = (private_node*)mymalloc(sizeof(private_node));
    if (new == NULL)
    {
        return -1;
    }

    node* new_pb = (node*)mymalloc(sizeof(node));
    if (new_pb == NULL)
    {
        free(new);
        return -1;
    }

    node tmp = {mymalloc(data_size), key};
    new_pb = memcpy(new_pb, &tmp, sizeof(node));
    if (new_pb->data_ == NULL)
    {
        free(new_pb);
        free(new);
        return -1;
    }

    new->key_ = key;
    new_pb->data_ = memcpy(new_pb->data_, data, data_size);
    new->data_ = new_pb;

    private_node* prev = NULL;
    private_node** rubish;
    private_node* cur = tree_search_private(tree, key, &prev, &rubish);
    if (cur != NULL)
    {
        free(new_pb->data_);
        free(new_pb);
        free(new);
        return -1;
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
    if (tree == NULL)
        return -1;

    private_node** prev_pos = NULL;
    private_node* rubish;
    private_node* cur = tree_search_private(tree, key, &rubish, &prev_pos);
    if (cur == NULL)
    {
        return -1;
    }

    if (cur->left_ == NULL && cur->right_ == NULL)
    {
        *prev_pos = NULL;
    }
    else if (cur->left_ != NULL && cur->right_ == NULL)
    {
        *prev_pos = cur->left_;
    }
    else if (cur->left_ == NULL && cur->right_ != NULL)
    {
        *prev_pos = cur->right_;
    }
    else
    {
        private_node* next = tree_min_private(cur->right_);
        if (next == cur->right_)
        {
            *prev_pos = next;
            next->left_ = cur->left_;
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
        }
    }

    free(cur->data_->data_);
    free(cur->data_);
    free(cur);
    return 0;
}

node* tree_search(tree* tree, int key)
{
    if (tree == NULL)
    {
        return NULL;
    }

    private_node* rubish1;
    private_node** rubish2;
    private_node* result = tree_search_private(tree, key, &rubish1, &rubish2);
    if (result == NULL)
    {
        return NULL;
    }

    return result->data_;
}

private_node* tree_search_private(tree* tree, int key, private_node** prev, private_node*** prev_pos)
{
    private_node* cur = tree->root_;
    while (cur != NULL && cur->key_ != key)
    {
        *prev = cur;
        if (key < cur->key_)
        {
            *prev_pos = &(cur->left_);
            cur = cur->left_;
        }
        else
        {
            *prev_pos = &(cur->right_);
            cur = cur->right_;
        }
    }

    return cur;
}

node* tree_min(tree* tree)
{
    if (tree == NULL)
        return NULL;

    private_node* result = tree_min_private(tree->root_);
    if (result == NULL)
    {
        return NULL;
    }

    return result->data_;
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

node* tree_max(tree* tree)
{
    if (tree == NULL)
        return NULL;

    private_node* result = tree_max_private(tree->root_);
    if (result == NULL)
    {
        return NULL;
    }

    return result->data_;
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

void tree_destroy(tree* tree)
{
    if (tree == NULL)
    {
        return;
    }

    if (tree->root_ != NULL)
    {
        tree_destroy_private(tree->root_);
    }

    free(tree);
}

void tree_destroy_private(private_node* root)
{
    if (root->left_ != NULL)
    {
        tree_destroy_private(root->left_);
    }

    if (root->right_ != NULL)
    {
        tree_destroy_private(root->right_);
    }

    free(root->data_->data_);
    free(root->data_);
    free(root);
}

int foreach(tree* tree, int(*func)(node* node, void* par), void* par)
{
    if (tree != NULL)
    {
        return foreach_private(tree->root_, func, par);
    }
    
    return -1;
}

int foreach_private(private_node* root, int(*func)(node* node, void* val), void* par)
{
    private_node* cur = root;
    int r1 = 0, r2 = 0, r3 = 0;
    if (cur != NULL)
    {
        r1 = foreach_private(cur->left_, func, par);
        r2 = func(cur->data_, par);
        r3 = foreach_private(cur->right_, func, par);
    }
    
    if (r1 == -1 || r2 == -1 || r3 == -1)
    {
        return -1;
    }
    
    return 0;
}