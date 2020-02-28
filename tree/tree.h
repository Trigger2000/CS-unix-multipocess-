#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct public_node
{
    void* data_;
    const int key_;
};

struct private_node;
struct tree;

typedef struct tree tree;
typedef struct private_node private_node;
typedef struct public_node node;

//Creates new tree
tree* new_tree();

//if already exist or tree=NULL returns -1 else returns 0
int tree_insert(tree* tree, int key, const void* data, int data_size);

//if doesn't exist or tree=NULL return -1 else returns 0
int tree_delete(tree* tree, int key);

//if no exist or tree=NULL returns NULL else returns pointer to data
node* tree_search(tree* tree, int key);

//Search min tree node. If tree is empty or tree=NULL returns NULL else returns pointer to data
node* tree_min(tree* tree);

//Search max tree node. If tree is empty or tree=NULL returns NULL else returns pointer to data
node* tree_max(tree* tree);

//Deletes all tree elements
void tree_destroy(tree* tree);

//Apply function func for each tree element via in-order traverse. Returns -1 if something is incorrect
int foreach(tree* tree, int(*func)(node* node, void* val), void* par);