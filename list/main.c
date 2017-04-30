#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "list.h"


typedef struct {
	int val;
	list_node_t node;
} sample_t;


list_t* list;
sample_t arr[10];


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
	assert(!list_begin(list));                                                \
	assert(!list_end(list));                                                  \
	test_size(0);

#define test_size_1()                                                         \
	test_size(1);                                                             \
	test_bounds();                                                            \
	assert(list->front == list->back);                                        \

#define test_size_2()                                                         \
	test_size(2);                                                             \
	test_bounds();                                                            \
	assert(list->front != list->back);                                        \
	assert(list->front->next == list->back);                                  \
	assert(list->back->prev  == list->front);

#define test_size_3()                                                         \
	test_size(3);                                                             \
	test_bounds();                                                            \
	assert(list->front != list->back);                                        \
	assert(list->front->next->next == list->back);                            \
	assert(list->back->prev->prev  == list->front);                           \
	assert(list->back->prev == list->front->next);


void test_create() {
	*list = list_create();
	assert(list);
	test_empty();
}


int get_value(list_iterator_t iter) {
	return list_entry(iter, sample_t, node)->val;
}
void test_print() {
	if (!list->size)
		return;
	for (list_iterator_t i = list_begin(list); i; i = list_iterator_next(i))
		printf("%d ", get_value(i));
	printf(" ");
	for (list_iterator_t i = list_end(list); i; i = list_iterator_prev(i))
		printf("%d ", get_value(i));
	printf("\n");
}


#define test_push(MODE)                                                       \
	assert(list_push_##MODE(list, &arr[0].node));                             \
	test_size_1();                                                            \
	assert(list_push_##MODE(list, &arr[1].node));                             \
	test_size_2();                                                            \
	assert(list_push_##MODE(list, &arr[2].node));                             \
	test_size_3();

#define test_pop(MODE)                                                        \
	assert(list_pop_##MODE(list));                                            \
	test_size_2();                                                            \
	assert(list_pop_##MODE(list));                                            \
	test_size_1();                                                            \
	assert(list_pop_##MODE(list));                                            \
	test_empty();                                                             \
	assert(!list_pop_##MODE(list));


#define test_null_2(F,A,B)                                                    \
	assert(!F(NULL, NULL));                                                   \
	assert(!F(A,    NULL));                                                   \
	assert(!F(NULL, B));
void test_null() {
	assert(!list_iterator_next(NULL));
	assert(!list_iterator_prev(NULL));

	assert(!list_begin(NULL));
	assert(!list_end(NULL));

	assert(!list_pop_front(NULL));
	assert(!list_pop_back(NULL));

	list_node_t* node = &arr[0].node;

	test_null_2(list_push_front, list, node);
	test_null_2(list_push_back,  list, node);
	test_null_2(list_erase,      list, node);
	test_null_2(list_size,       list, &size);

	assert(!list_insert(NULL, NULL, NULL));
	assert(!list_insert(list, NULL, NULL));
	assert(!list_insert(NULL, node, NULL));
	assert(!list_insert(NULL, NULL, node));
	assert(!list_insert(list, node, NULL));
	assert(!list_insert(NULL, node, node));
	assert(!list_insert(list, NULL, node));
}


void test_select() {
	assert(list_push_back(list, &arr[0].node));
	assert(list_insert(list, list_begin(list), &arr[1].node));
	test_size_2();
	assert(list_insert(list, list_end(list),   &arr[2].node));
	test_size_3();
	test_print();
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
