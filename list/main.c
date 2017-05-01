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
	assert(list_size(list) == EXPECTED);                                      \
	assert(list->size      == EXPECTED);

#define test_bounds()                                                         \
	assert(!list->front->prev);                                               \
	assert(!list->back-> next);


#define test_empty()                                                          \
	assert(!list->front); assert(!list->back);                                \
	assert(!list_iterator_nil(list_begin(list)));                             \
	assert(!list_iterator_nil(list_end  (list)));                             \
	test_size(0);

ListNode* curr;
#define test_list(SIZE)                                                       \
	test_size(SIZE);                                                          \
	test_bounds();                                                            \
	curr = list->front;                                                       \
	for (int test = 0; test < SIZE-1; ++test) curr = curr->next;              \
	assert(curr == list->back);                                               \
	curr = list->back;                                                        \
	for (int test = 0; test < SIZE-1; ++test) curr = curr->prev;              \
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

	for (ListIterator i = list_begin(list);
			list_iterator_cmp(i, list_end(list)); list_iterator_next(&i))
		printf("%d ", get_value(i));
	printf("\n");

	// Can't do reverse order!
}


#define test_push(MODE)                                                       \
	assert(list_push_##MODE(list, &arr[0].node)); test_list(1);               \
	assert(list_push_##MODE(list, &arr[1].node)); test_list(2);               \
	assert(list_push_##MODE(list, &arr[2].node)); test_list(3);

#define test_pop(MODE)                                                        \
	assert(list_pop_##MODE(list)); test_list(2);                              \
	assert(list_pop_##MODE(list)); test_list(1);                              \
	assert(list_pop_##MODE(list)); test_empty();                              \
	assert(!list_pop_##MODE(list));


#define test_null_2(F,A,An,B,Bn)                                              \
	assert(!F(An, Bn));                                                       \
	assert(!F(A,  Bn));                                                       \
	assert(!F(An, B ));
void test_null() {
	// Test nil iterators
	assert(!list_iterator_nil(ListIteratorNil));
	assert(!list_iterator_nil(list_iterator_next(NULL)));
	assert(!list_iterator_nil(list_iterator_prev(NULL)));
	assert(!list_iterator_nil(list_begin(NULL)));
	assert(!list_iterator_nil(list_end  (NULL)));

	// Test constraints
	ListIterator iter = list_begin(list);
	list_iterator_prev(&iter);
	assert(!list_iterator_nil(list_iterator_prev(&iter)));
	iter = list_end(list);
	assert(!list_iterator_nil(list_iterator_next(&iter)));
	assert(!list_erase(list, list_end(list)));

	// Test NULL inputs
	assert(!list_pop_front(NULL));
	assert(!list_pop_back (NULL));
	assert(!list_size(NULL));

	ListNode* node = &arr[0].node;
	iter = list_begin(list);

	test_null_2(list_push_front, list, NULL, node, NULL);
	test_null_2(list_push_back,  list, NULL, node, NULL);
	test_null_2(list_erase,      list, NULL, iter, ListIteratorNil);

	assert(!list_insert(NULL, ListIteratorNil, NULL));
	assert(!list_insert(list, ListIteratorNil, NULL));
	assert(!list_insert(NULL, iter,            NULL));
	assert(!list_insert(NULL, ListIteratorNil, node));
	assert(!list_insert(list, iter,            NULL));
	assert(!list_insert(NULL, iter,            node));
	assert(!list_insert(list, ListIteratorNil, node));
}


void test_select() {
	// Insert front/back
	list_push_back(list, &arr[0].node);
	assert(list_insert(list, list_begin(list), &arr[1].node)); test_size(2);
	assert(list_insert(list, list_end(list),   &arr[2].node)); test_size(3);

	// Erase front/back
	ListIterator iter = list_end(list);
	assert(list_erase (list, list_begin(list)));               test_size(2);
	assert(list_erase (list, list_iterator_prev(&iter)));      test_size(1);

	list_push_back(list, &arr[1].node);
	list_push_back(list, &arr[2].node);
	// Arbitrary nsert/erase
	iter = list_begin(list);
	assert(list_insert(list, list_iterator_next(&iter), &arr[3].node));
	                                                           test_size(4);
	assert(list_erase (list, iter));                           test_size(3);
}


int main() {
	list = malloc(sizeof(list));
	arr[0].val = 1; arr[1].val = 2;
	arr[2].val = 3; arr[3].val = 4;

	test_create();

	test_push(front);
	test_print();
	test_null();
	test_pop(front);

	test_push(back);
	test_pop(back);

	test_select();

	free(list);
	return 0;
}
