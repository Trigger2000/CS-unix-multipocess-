#include "tree.h"

void test_insert(tree* tree_);
void test_search(tree* tree_);
void test_min_max(tree* tree_);
void test_search_next(tree* tree_);
void test_delete(tree* tree_);
int func(node* node_)
{
    printf("%c", node_->data_);
}

int main()
{
    tree* example = new_tree();
    tree* example1 = new_tree();
    tree* example2 = new_tree();
    tree* broken = new_tree();
    assert(broken == NULL);
    tree_destroy(&example1);
    tree_destroy(&example2);

    test_insert(example);
    assert(foreach(example, func) == 13);
    assert(foreach(NULL, func) == -1);
    printf("\n");
    test_search(example);
    test_min_max(example);
    test_search_next(example);
    test_delete(example);
    tree_destroy(&example);
    print(example);
    printf("\n");

    return 0;
}

void test_insert(tree* tree_)
{
    node tmp = {'a'};
    tree_insert(NULL, 7, &tmp);
    tree_insert(tree_, 7, &tmp);
    tree_insert(tree_, 7, &tmp);
    tree_insert(tree_, 3, &tmp);
    tree_insert(tree_, 3, &tmp);
    tree_insert(tree_, 5, &tmp);
    tree_insert(tree_, 5, &tmp);
    tree_insert(tree_, -2, &tmp);
    tree_insert(tree_, -2, &tmp);
    tree_insert(tree_, 0, &tmp);
    tree_insert(tree_, 0, &tmp);
    tree_insert(tree_, 2, &tmp);
    tree_insert(tree_, 2, &tmp);
    tree_insert(tree_, 3, &tmp);
    tree_insert(tree_, 15, &tmp);
    tree_insert(tree_, 15, &tmp);
    tree_insert(tree_, 13, &tmp);
    tree_insert(tree_, 13, &tmp);
    tree_insert(tree_, 17, &tmp);
    tree_insert(tree_, 17, &tmp);
    tree_insert(tree_, 13, &tmp);
    tree_insert(tree_, -1, &tmp);
    tree_insert(tree_, -1, &tmp);
    tree_insert(tree_, 14, &tmp);
    tree_insert(tree_, 14, &tmp);
    tree_insert(tree_, 4, &tmp);
    tree_insert(tree_, 4, &tmp);
    tree_insert(tree_, 16, &tmp);
    tree_insert(tree_, 16, &tmp);
    print(tree_);
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
    errno = 0;
    tree* tmp = new_tree();
    assert(tree_min(NULL) == -1);
    assert(tree_max(NULL) == -1);
    assert(tree_min(tree_) == -2);
    assert(tree_max(tree_) == 17);
    assert(tree_min(tmp) == -1);
    assert(tree_max(tmp) == -1);
    assert(errno == EINVAL);

    if (tmp != NULL)
        tree_destroy(&tmp);
}

void test_search_next(tree* tree_)
{
    errno = 0;
    assert(search_next(NULL, 0) == -1);
    assert(search_next(tree_, 25) == -1);
    assert(search_next(tree_, -1) == 0);
    assert(search_next(tree_, 5) == 7);
    assert(search_next(tree_, 3) == 4);
    assert(errno == EINVAL);
}

void test_delete(tree* tree_)
{
    assert(tree_delete(NULL, 25) == -1);
    assert(tree_delete(tree_, 25) == -1);

    node tmp = {'a'};
    tree_delete(tree_, 14);
    printf("no 14 ");
    print(tree_);
    printf("\n");
    tree_insert(tree_, 14, &tmp);
    tree_insert(tree_, 14, &tmp);

    tree_delete(tree_, 3);
    printf("no 3 ");
    print(tree_);
    printf("\n");
    tree_insert(tree_, 3, &tmp);
    tree_insert(tree_, 3, &tmp);

    tree_delete(tree_, 13);
    printf("no 13 ");
    print(tree_);
    printf("\n");
    tree_insert(tree_, 13, &tmp);
    tree_insert(tree_, 13, &tmp);

    tree_delete(tree_, 5);
    printf("no 5 ");
    print(tree_);
    printf("\n");
    tree_insert(tree_, 5, &tmp);
    tree_insert(tree_, 5, &tmp);

    tree_delete(tree_, 17);
    printf("no 17 ");
    print(tree_);
    printf("\n");
    tree_insert(tree_, 17, &tmp);
    tree_insert(tree_, 17, &tmp);

    tree_delete(tree_, 15);
    printf("no 15 ");
    print(tree_);
    printf("\n");
    tree_insert(tree_, 15, &tmp);
    tree_insert(tree_, 15, &tmp);

    tree_delete(tree_, 7);
    printf("no 7 ");
    print(tree_);
    printf("\n");
    tree_insert(tree_, 7, &tmp);
    tree_insert(tree_, 7, &tmp);
}