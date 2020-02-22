#include "tree.h"

void test_insert(tree* tree);
void test_search(tree* tree);
void test_min_max(tree* tree);
void test_search_next(tree* tree);
//void test_delete(tree* tree);

int main()
{
    tree* example = new_tree();
    test_insert(example);
    test_search(example);
    test_min_max(example);
    test_search_next(example);
    //test_delete(&example);

    //tree_destroy(example->root_);
    return 0;
}

void test_insert(tree* tree)
{
    node tmp = {'a'};
    tree_insert(tree, 7, &tmp);
    tree_insert(tree, 3, &tmp);
    tree_insert(tree, 5, &tmp);
    tree_insert(tree, -2, &tmp);
    tree_insert(tree, 0, &tmp);
    tree_insert(tree, 2, &tmp);
    tree_insert(tree, 3, &tmp);
    tree_insert(tree, 15, &tmp);
    tree_insert(tree, 13, &tmp);
    tree_insert(tree, 17, &tmp);
    tree_insert(tree, 13, &tmp);
    tree_insert(tree, -1, &tmp);
    tree_insert(tree, 14, &tmp);
    tree_insert(tree, 4, &tmp);
    print(tree);
    printf("\n");
}

void test_search(tree* tree)
{
    node* node1 = tree_search(tree, 7);
    node* node2 = tree_search(tree, 0);
    node* node3 = tree_search(tree, 5);
    node* node4 = tree_search(tree, 14);
    node* node5 = tree_search(tree, 25);
    assert(node1 != NULL);
    assert(node2 != NULL);
    assert(node3 != NULL);
    assert(node4 != NULL);
    assert(node5 == NULL);
}

void test_min_max(tree* tree)
{
    int min = tree_min(tree);
    int max = tree_max(tree);
    assert(min == -2);
    assert(max == 17);
}

void test_search_next(tree* tree)
{
    int node1 = search_next(tree, 25);
    int node2 = search_next(tree, -1);
    int node3 = search_next(tree, 5);
    assert(node1 == -1);
    assert(errno == EINVAL);
    assert(node2 == 0);
    assert(node 3 == 7);
}

/*void test_delete(tree* tree)
{
    assert(tree_delete(tree, 25) == -1);
    tree_delete(tree, 14);
    print(tree->root_);
    printf("\n");
    tree_insert(tree, 14);

    tree_delete(tree, 13);
    print(tree->root_);
    printf("\n");
    tree_insert(tree, 13);

    tree_delete(tree, 5);
    print(tree->root_);
    printf("\n");
    tree_insert(tree, 5);

    tree_delete(tree, 15);
    print(tree->root_);
    printf("\n");
    tree_insert(tree, 15);

    tree_delete(tree, 7);
    print(tree->root_);
    printf("\n");
    tree_insert(tree, 7);
} */