#include <stdio.h>
#include <stdlib.h>

struct tree
{
    node* root_;
}

struct node
{
    node* prev_ = nullptr, left_ = nullptr, right_ = nullptr;
    int data_ = 0;
}

void tree_insert(tree* tree, node* key);
void tree_delete(tree* tree, node* key);
node* tree_search(tree* tree, int key);
node* seach_next(tree* tree, int key);
node* tree_min(node* root_);
node* tree_max(node* root_);
void print(node* root);