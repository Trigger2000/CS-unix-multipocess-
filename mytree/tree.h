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
node* tree_search(int key);