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


void test_print() {
	if (!list->size)
		return;

	for (ListIterator i = list_begin(list);
			list_iterator_cmp(i, list_end(list)); list_iterator_next(&i))
		printf("%d ", *(int*)list_entry(i));
	printf("\n");
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


int main() {
	list = malloc(sizeof(list));
	arr[0].val = 1; arr[1].val = 2; arr[2].val = 3;

	test_create();
	test_push(front);
	test_print();
	test_pop(front);
	test_push(back);
	test_pop(back);

	free(list);
	return 0;
}
