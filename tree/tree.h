#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>

struct public_node
{
    char data_;
};

struct private_node;
struct tree;

typedef struct tree tree;
typedef struct private_node private_node;
typedef struct public_node node;

//Creates new tree
tree* new_tree();

//if already exist returns -1 else returns 0
int tree_insert(tree* tree, int key, node* data_node);

//if doesn't exist return -1 else returns 0
int tree_delete(tree* tree, int key);

//if no exist returns NULL
node* tree_search(tree* tree, int key);
static private_node* tree_search_private(tree* tree, int key);

//Finds private_node with min key_value greater than key. if no exist returns NULL
int search_next(tree* tree, int key);
static private_node* search_next_private(tree* tree, int key);

//Search min tree element, starting with private_node root. If tree is empty returns NULL
int tree_min(tree* tree);
static private_node* tree_min_private(private_node* root);

//Search max tree element, starting with private_node root. If tree is empty returns NULL
int tree_max(tree* tree);
static private_node* tree_max_private(private_node* root);

//Prints tree elements, starting with private_node root
void print(tree* tree);
static void print_private(private_node* root);

//Delete tree elements, starting with private_node root
void tree_destroy(tree* tree);
static void tree_destroy_private(private_node* root);