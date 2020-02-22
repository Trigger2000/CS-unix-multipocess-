#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

struct node;
struct tree;

typedef struct tree tree;
typedef struct node node;

//Creates new tree
tree* new_tree();

//if already exist returns -1 else returns 0
int tree_insert(tree* tree, int key);

//if doesn't exist return -1 else returns 0
int tree_delete(tree* tree, int key);

//if no exist returns NULL
node* tree_search(tree* tree, int key);

//Finds node with min key_value greater than key. if no exist returns NULL
node* search_next(tree* tree, int key);

//Search min tree element, starting with node root. If tree is empty returns NULL
node* tree_min(tree* tree);
static node* tree_min_private(node* root);

//Search max tree element, starting with node root. If tree is empty returns NULL
node* tree_max(tree* tree);
static node* tree_max_private(node* root);

//Prints tree elements, starting with node root
void print(tree* tree);
static void print_private(node* root);

//Delete tree elements, starting with node root
void tree_destroy(tree* tree);
static void tree_destroy_private(node* root);