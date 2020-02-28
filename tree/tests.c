#include "tree.h"
#include <assert.h>

void test_allocation();
void test_insert(tree* tree_);
void test_search(tree* tree_);
void test_min_max(tree* tree_);
void test_delete(tree* tree_);

int print(node* node, void* val)
{
    printf("{%d, %c} ", node->key_, *(char*)(node->data_));
    return 0;
}

int fault(node* node, void* val)
{
    if (node->key_ != 13)
    {
        return 0;
    }
    return -1;
}

int main()
{
    tree* example = new_tree();
    test_allocation();
    test_insert(example);
    test_search(example);
    test_min_max(example);
    test_delete(example);
    tree_destroy(example);

    return 0;
}

void test_allocation()
{
    tree* ex1 = new_tree();
    tree* ex2 = new_tree();
    tree* ex3 = new_tree();
    tree* ex4 = new_tree();
    tree* ex5 = new_tree();
    tree_destroy(ex1);
    tree_destroy(ex2);
    tree_destroy(ex3);
    tree_destroy(ex4);
    tree_destroy(ex5);
    foreach(NULL, print, NULL);
}

void test_insert(tree* tree_)
{
    char tmp = 'a';
    tree_insert(NULL, 7, &tmp, sizeof(tmp));
    tree_insert(tree_, 7, &tmp, sizeof(tmp));
    tree_insert(tree_, 7, &tmp, sizeof(tmp));
    tree_insert(tree_, 3, &tmp, sizeof(tmp));
    tree_insert(tree_, 3, &tmp, sizeof(tmp));
    tree_insert(tree_, 5, &tmp, sizeof(tmp));
    tree_insert(tree_, 5, &tmp, sizeof(tmp));
    tree_insert(tree_, -2, &tmp, sizeof(tmp));
    tree_insert(tree_, -2, &tmp, sizeof(tmp));
    tree_insert(tree_, 0, &tmp, sizeof(tmp));
    tree_insert(tree_, 0, &tmp, sizeof(tmp));
    tree_insert(tree_, 2, &tmp, sizeof(tmp));
    tree_insert(tree_, 2, &tmp, sizeof(tmp));
    tree_insert(tree_, 3, &tmp, sizeof(tmp));
    tree_insert(tree_, 15, &tmp, sizeof(tmp));
    tree_insert(tree_, 15, &tmp, sizeof(tmp));
    tree_insert(tree_, 13, &tmp, sizeof(tmp));
    tree_insert(tree_, 13, &tmp, sizeof(tmp));
    tree_insert(tree_, 17, &tmp, sizeof(tmp));
    tree_insert(tree_, 17, &tmp, sizeof(tmp));
    tree_insert(tree_, 13, &tmp, sizeof(tmp));
    tree_insert(tree_, -1, &tmp, sizeof(tmp));
    tree_insert(tree_, -1, &tmp, sizeof(tmp));
    tree_insert(tree_, 14, &tmp, sizeof(tmp));
    tree_insert(tree_, 14, &tmp, sizeof(tmp));
    tree_insert(tree_, 4, &tmp, sizeof(tmp));
    tree_insert(tree_, 4, &tmp, sizeof(tmp));
    tree_insert(tree_, 16, &tmp, sizeof(tmp));
    tree_insert(tree_, 16, &tmp, sizeof(tmp));
    foreach(tree_, print, NULL);
    foreach(tree_, fault, NULL);
    printf("\n");
}

void test_search(tree* tree_)
{
    assert(tree_search(NULL, 7) == NULL);
    assert(tree_search(tree_, 7) != NULL);
    assert(tree_search(tree_, 0) != NULL);
    assert(tree_search(tree_, 5) != NULL);
    assert(tree_search(tree_, 14) != NULL);
    assert(tree_search(tree_, 25) == NULL);
}

void test_min_max(tree* tree_)
{
    tree* tmp = new_tree();
    assert(tree_min(NULL) == NULL);
    assert(tree_max(NULL) == NULL);
    assert(tree_min(tree_)->key_ == -2);
    assert(tree_max(tree_)->key_ == 17);
    assert(tree_min(tmp) == NULL);
    assert(tree_max(tmp) == NULL);
    tree_destroy(tmp);
}

void test_delete(tree* tree_)
{
    assert(tree_delete(NULL, 25) == -1);
    assert(tree_delete(tree_, 25) == -1);

    char tmp = 'a';
    tree_delete(tree_, 14);
    printf("no 14 ");
    foreach(tree_, print, NULL);
    printf("\n");
    tree_insert(tree_, 14, &tmp, sizeof(tmp));
    tree_insert(tree_, 14, &tmp, sizeof(tmp));

    tree_delete(tree_, 3);
    printf("no 3 ");
    foreach(tree_, print, NULL);
    printf("\n");
    tree_insert(tree_, 3, &tmp, sizeof(tmp));
    tree_insert(tree_, 3, &tmp, sizeof(tmp));

    tree_delete(tree_, 13);
    printf("no 13 ");
    foreach(tree_, print, NULL);
    printf("\n");
    tree_insert(tree_, 13, &tmp, sizeof(tmp));
    tree_insert(tree_, 13, &tmp, sizeof(tmp));

    tree_delete(tree_, 5);
    printf("no 5 ");
    foreach(tree_, print, NULL);
    printf("\n");
    tree_insert(tree_, 5, &tmp, sizeof(tmp));
    tree_insert(tree_, 5, &tmp, sizeof(tmp));

    tree_delete(tree_, 17);
    printf("no 17 ");
    foreach(tree_, print, NULL);
    printf("\n");
    tree_insert(tree_, 17, &tmp, sizeof(tmp));
    tree_insert(tree_, 17, &tmp, sizeof(tmp));

    tree_delete(tree_, 15);
    printf("no 15 ");
    foreach(tree_, print, NULL);
    printf("\n");
    tree_insert(tree_, 15, &tmp, sizeof(tmp));
    tree_insert(tree_, 15, &tmp, sizeof(tmp));

    tree_delete(tree_, 7);
    printf("no 7 ");
    foreach(tree_, print, NULL);
    printf("\n");
    tree_insert(tree_, 7, &tmp, sizeof(tmp));
    tree_insert(tree_, 7, &tmp, sizeof(tmp));
}