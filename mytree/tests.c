#include "tree.h"

void test_insert(tree* tree);
void test_search(tree* tree);
void test_min_max(tree* tree);
void test_search_next(tree* tree);
void test_delete(tree* tree);

int main()
{
    tree* example = (tree*)calloc(1, sizeof(tree));
    test_insert(example);
    test_search(example);
    test_min_max(example);
    test_search_next(example);
    test_delete(example);

    tree_destroy(example->root_);
    print(example->root_);
    printf("\n");

    return 0;
}

void test_insert(tree* tree)
{
    tree_insert(tree, 7);
    //print(tree->root_);
    //printf("\n");
    tree_insert(tree, 3);
    tree_insert(tree, 5);
    tree_insert(tree, -2);
    //print(tree->root_);
    //printf("\n");
    tree_insert(tree, 0);
    tree_insert(tree, 2);
    tree_insert(tree, 3);
    tree_insert(tree, 15);
    tree_insert(tree, 13);
    tree_insert(tree, 17);
    tree_insert(tree, 13);
    tree_insert(tree, -1);
    tree_insert(tree, 14);
    tree_insert(tree, 4);
    print(tree->root_);
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
    node* min = tree_min(tree->root_);
    node* max = tree_max(tree->root_);
    node* empty1 = tree_min(tree->root_->right_->right_->right_);
    node* empty2 = tree_max(tree->root_->right_->right_->right_);
    assert(min->data_ == -2);
    assert(max->data_ == 17);
    assert(empty1 == NULL);
    assert(empty2 == NULL);
}

void test_search_next(tree* tree)
{
    node* node1 = search_next(tree, 25);
    node* node2 = search_next(tree, -2);
    node* node3 = search_next(tree, 5);
    assert(node1 == NULL);
    assert(node2->data_ == -1);
    assert(node3->data_ == 7);
}

void test_delete(tree* tree)
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
}