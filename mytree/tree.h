#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

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

typedef struct tree tree;
typedef struct node node;

//if already exist returns -1 else returns 0
int tree_insert(tree* tree, int key); //x

//if doesn't exist return -1 else returns 0
int tree_delete(tree* tree, int key); //x

//if no exist returns nullptr
node* tree_search(tree* tree, int key); //x

//Finds node with min key_value greater than key. if no exist returns nullptr
node* search_next(tree* tree, int key); //x

//Search min tree element, starting with node root_. If no exist returns nullptr
node* tree_min(node* root_); //x

//Search max tree element, starting with node root_. If no exist returns nullptr
node* tree_max(node* root_); //x

//Prints tree elements, starting with node root
void print(node* root); //x

//Delete tree elements, starting with node root
void tree_destroy(node* root);