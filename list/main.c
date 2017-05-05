#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "list.h"


typedef struct {
    int val;
    ListNode node;
} Sample;


#define test_size(EXPECTED)                                                   \
    assert(list_size(list) == EXPECTED);                                      \
    assert(list->size      == EXPECTED);

#define test_bounds()                                                         \
    assert(!list->front->prev);                                               \
    assert(!list->back-> next);


#define test_empty()                                                          \
    assert(!list->front); assert(!list->back);                                \
    assert(list_iterator_nil(list_begin(list)));                              \
    assert(list_iterator_nil(list_end  (list)));                              \
    test_size(0);


#define test_list(SIZE)                                                       \
    test_size(SIZE);                                                          \
    test_bounds();                                                            \
    cur_node = list->front;                                                   \
    for (int test = 0; test < SIZE-1; ++test) cur_node = cur_node->next;      \
    assert(cur_node == list->back);                                           \
    cur_node = list->back;                                                    \
    for (int test = 0; test < SIZE-1; ++test) cur_node = cur_node->prev;      \
    assert(cur_node == list->front);


#define test_create()                                                         \
    *list = list_create();                                                    \
    test_empty();

#define test_print()                                                          \
    printf("Straight order: ");                                               \
    for (ListIterator i = list_begin(list);                                   \
            list_iterator_cmp(i, list_end(list)); list_iterator_next(&i)) {   \
        printf("%d ", *(int*)list_entry(i, Sample, node));                    \
    }                                                                         \
    printf("\n");                                                             \
    printf("Reverse order: ");                                                \
    for (ListIterator i = list_iterator_advance(list_end(list), -1);          \
            list_iterator_cmp(i, list_begin(list)); list_iterator_prev(&i)) { \
        printf("%d ", *(int*)list_entry(i, Sample, node));                    \
    }                                                                         \
    printf("%d ", *(int*)list_entry(list_begin(list), Sample, node));         \
    printf("\n");

#define test_push(MODE)                                                       \
    assert(list_push_##MODE(list, &arr[0].node)); test_list(1);               \
    assert(list_push_##MODE(list, &arr[1].node)); test_list(2);               \
    assert(list_push_##MODE(list, &arr[2].node)); test_list(3);

#define test_pop(MODE)                                                        \
    assert(list_pop_##MODE(list)); test_list(2);                              \
    assert(list_pop_##MODE(list)); test_list(1);                              \
    assert(list_pop_##MODE(list)); test_empty();                              \
    assert(!list_pop_##MODE(list));

#define test_nils()                                                           \
    cur_iter = ListIteratorNil;                                               \
    assert(list_iterator_nil(list_iterator_next(&cur_iter)));                 \
    assert(list_iterator_nil(list_iterator_next(NULL)));                      \
    assert(list_iterator_nil(list_iterator_prev(&cur_iter)));                 \
    assert(list_iterator_nil(list_iterator_prev(NULL)));                      \
    cur_iter = (ListIterator){NULL, NULL, NULL, 1};                           \
    assert(list_iterator_nil(list_iterator_next(&cur_iter)));                 \
    cur_iter = (ListIterator){NULL, NULL, NULL, 1};                           \
    assert(list_iterator_nil(list_iterator_prev(&cur_iter)));

int main() {
    List* list = calloc(1, sizeof(list));
    Sample arr[10];
    ListNode* cur_node;
    ListIterator cur_iter;
    arr[0].val = 1; arr[1].val = 2; arr[2].val = 3;

    test_create();
    test_push(front);
    test_print();
    test_pop(front);

    arr[0].val = 3; arr[1].val = 4; arr[2].val = 5;

    test_push(back);
    test_print();
    test_pop(back);

    test_nils();

    free(list);
    return 0;
}
