#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "list.h"


typedef struct {
	int val;
	ListNode node;
} Sample;


List* list;
Sample arr[10];


size_t size;
#define test_size(EXPECTED)                                                   \
	assert(list_size(list, &size));                                           \
	assert(size == EXPECTED);

#define test_bounds()                                                         \
	assert(!list->front->prev);                                               \
	assert(!list->back->next);


#define test_empty()                                                          \
	assert(!list->front);                                                     \
	assert(!list->back);                                                      \
	assert(!list_begin(list).node);                                           \
	assert(!list_end(list).node);                                             \
	test_size(0);

size_t test;
ListNode* curr;
#define test_list(SIZE)                                                       \
	test_size(SIZE);                                                          \
	test_bounds();                                                            \
	curr = list->front;                                                       \
	for (int test = 0; test < SIZE-1; ++test)                                 \
		curr = curr->next;                                                    \
	assert(curr == list->back);                                               \
	curr = list->back;                                                        \
	for (int test = 0; test < SIZE-1; ++test)                                 \
		curr = curr->prev;                                                    \
	assert(curr == list->front);


void test_create() {
	*list = list_create();
	assert(list);
	test_empty();
}


int get_value(ListIterator iter) {
	return list_entry(iter.node, Sample, node)->val;
}
void test_print() {
	if (!list->size)
		return;

	for (ListIterator i = list_begin(list); !list_iterator_cmp(i, list_end(list)); list_iterator_next(&i))
		printf("%d ", get_value(i));
	printf(" ");

	// !Can't do reverse order
}


#define test_push(MODE)                                                       \
	assert(list_push_##MODE(list, &arr[0].node));                             \
	test_list(1);                                                             \
	assert(list_push_##MODE(list, &arr[1].node));                             \
	test_list(2);                                                             \
	assert(list_push_##MODE(list, &arr[2].node));                             \
	test_list(3);

#define test_pop(MODE)                                                        \
	assert(list_pop_##MODE(list));                                            \
	test_list(2);                                                             \
	assert(list_pop_##MODE(list));                                            \
	test_list(1);                                                             \
	assert(list_pop_##MODE(list));                                            \
	test_empty();                                                             \
	assert(!list_pop_##MODE(list));


#define test_null_2(F,A,B)                                                    \
	assert(!F(NULL, NULL));                                                   \
	assert(!F(A,    NULL));                                                   \
	assert(!F(NULL, B));
void test_null() {
	// assert(!list_iterator_next(NULL));
	// assert(!list_iterator_prev(NULL));

	assert(!list_begin(NULL).node);
	// assert(!list_end(NULL).node);

	assert(!list_pop_front(NULL));
	assert(!list_pop_back(NULL));

	ListNode* node = &arr[0].node;
	// ListIterator nitr = list_begin(NULL);

	test_null_2(list_push_front, list, node);
	test_null_2(list_push_back,  list, node);
	// test_null_2(list_erase,      list, nitr);
	test_null_2(list_size,       list, &size);

	// assert(!list_insert(NULL, NULL, NULL));
	// assert(!list_insert(list, NULL, NULL));
	// assert(!list_insert(NULL, nitr, NULL));
	// assert(!list_insert(NULL, NULL, node));
	// assert(!list_insert(list, nitr, NULL));
	// assert(!list_insert(NULL, nitr, node));
	// assert(!list_insert(list, NULL, node));
}


void test_select() {
	// list_push_back(list, &arr[0].node);
	// assert(list_insert(list, list_begin(list), &arr[1].node));
	// test_size_2();
	// assert(list_insert(list, list_end(list),   &arr[2].node));
	// test_size_3();

	// assert(list_erase(list, list_begin(list)));
	// test_size_2();
	// assert(list_erase(list, list_end(list)));
	// test_size_1();

	// list_push_back(list, &arr[1].node);
	// list_push_back(list, &arr[0].node);
	// ListIterator lbegin = list_begin(list);
	// assert(list_erase(list, list_iterator_next(&lbegin)));
	// test_size_2();
}


int main() {
	list = malloc(sizeof(list));
	arr[0].val = 1;
	arr[1].val = 2;
	arr[2].val = 3;

	test_create();

	test_push(front);
	test_print();
	test_null();
	test_pop(front);

	test_push(back);
	test_print();
	test_pop(back);

	test_push(front);
	test_pop(back);
	test_push(back);
	test_pop(front);

	test_select();

	free(list);
	return 0;
}
