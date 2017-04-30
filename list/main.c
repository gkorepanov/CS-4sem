#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "list.h"

#define ARR_SIZE  256
typedef struct {
	int val;
	list_node_t node;
} sample_t;


int main() {
	// Check list creation
	list_t list = list_create();
		assert(!list.front);
		assert(!list.back);
		assert(!list.size);

	// Check push_front
	sample_t sample[ARR_SIZE];
	sample[0].val = LIST_OK;
	assert(list_push_front(&list, &sample[0].node));
		assert(!list_begin(&list)->prev);
		assert(!list_begin(&list)->next);
		assert(!list_end(&list)->prev);
		assert(!list_end(&list)->next);
		assert(list_begin(&list) == list_end(&list));
		assert(list.size == 1);

	// Check obtaining the element
	sample_t *elem = list_entry(list_begin(&list), sample_t, node);
		assert(elem);
		assert(elem->val == sample[0].val);

	// Check pop_front
	assert(list_pop_front(&list));
		assert(!list.front);
		assert(!list.back);
		assert(!list.size);
		assert(!list_pop_front(&list));

	// Check multiple push_front
	sample[1].val = LIST_OK + 1;
	sample[2].val = LIST_OK + 2;
	assert(list_push_front(&list, &sample[0].node));
	assert(list_push_front(&list, &sample[1].node));
	assert(list_push_front(&list, &sample[2].node));
		assert(list_entry(list_begin(&list), sample_t, node)->val == sample[2].val);
		assert(list_entry(list_end(&list),   sample_t, node)->val == sample[0].val);
		assert(!list_begin(&list)->prev);
		assert(!list_end(&list)->next);
		assert(list_begin(&list)->next->next == list_end(&list));
		assert(list_end(&list)->prev->prev == list_begin(&list));
		assert(list.size == 3);
	// and size()
	size_t size;
		assert(list_size(&list, &size));
		assert(size == list.size);

	printf("Test PASSED\n");
	return 0;
}
