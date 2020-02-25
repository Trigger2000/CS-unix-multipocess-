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

//Fault injection malloc
void* mymalloc(int size);

//Creates new tree
tree* new_tree();

//if already exist or tree=NULL returns -1 else returns 0
int tree_insert(tree* tree, int key, node* data_node);

//if doesn't exist or tree=NULL return -1 else returns 0
int tree_delete(tree* tree, int key);

//if no exist or tree=NULL returns NULL
node* tree_search(tree* tree, int key);
//prev - parent node, prev_pos - position relatively to parent node
static private_node* tree_search_private(tree* tree, int key, private_node** prev, private_node*** prev_pos);

//Finds node's key with min value greater than key. if no exist or tree=NULL returns -1 setting errno EINVAL
int search_next(tree* tree, int key);
static private_node* search_next_private(tree* tree, int key);

//Search min tree element. If tree is empty or tree=NULL returns -1 setting errno EINVAL
int tree_min(tree* tree);
static private_node* tree_min_private(private_node* root);

//Search max tree element. If tree is empty or tree=NULL returns -1 setting errno EINVAL
int tree_max(tree* tree);
static private_node* tree_max_private(private_node* root);

//Prints tree elements
void print(tree* tree);
static void print_private(private_node* root);

//Deletes tree elements
void tree_destroy(tree** tree);
static void tree_destroy_private(private_node* root);

//Apply function func for each tree element. Returns how many functions func were applied. If tree=NULL returns -1
int foreach(tree* tree, int(*func)(node* node));
static int foreach_private(private_node* root, int(*func)(node* node));