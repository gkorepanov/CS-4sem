#include "list.h"
#include <string.h>


List list_create() {
	return (List) {
		NULL,
		NULL,
		0
	};
}


const ListIterator ListIteratorNil = {NULL, NULL, NULL, 0};

int list_iterator_nil(ListIterator iter) {
	return (iter.prev || iter.node || iter.prev || iter.pos);
}

ListIterator list_iterator_next(ListIterator* iter) {
	if (!iter || !list_iterator_nil(*iter))
		return ListIteratorNil;

	if (iter->node)
		*iter = (ListIterator) {
			iter->node,
			iter->next,
			iter->next ? iter->next->next : NULL,
			iter->pos + 1
		};
	else
		*iter = ListIteratorNil;

	return *iter;
}

ListIterator list_iterator_prev(ListIterator* iter) {
	if (!iter || !list_iterator_nil(*iter))
		return ListIteratorNil;

	if (iter->prev)
		*iter = (ListIterator) {
			iter->prev->prev,
			iter->prev,
			iter->node,
			iter->pos - 1
		};
	else
		*iter = ListIteratorNil;

	return *iter;
}

ListIterator list_iterator_advance(ListIterator iter, int step) {
	ListIterator (*step_over)(ListIterator*) =
		((step >= 0) ? &list_iterator_next : &list_iterator_prev);
	for (step = ((step >= 0) ? step : -step) ; step; --step)
		(*step_over)(&iter);
	return iter;
}

int list_iterator_cmp(ListIterator a, ListIterator b) {
	return (a.pos == b.pos) ? 0 : ((a.pos > b.pos) ? 1 : -1);
}

void* list_entry(ListIterator iter) {
	return (void*)iter.node;
}

void list_for_each(ListIterator iter1, ListIterator iter2,
	               void (*func)(void*)) {
	ListIterator (*step_over)(ListIterator*) =
		((list_iterator_cmp(iter1, iter2) >= 0)
			? &list_iterator_next
			: &list_iterator_prev);
	for (; list_iterator_cmp(iter1, iter2); (*step_over)(&iter1))
		(*func)(list_entry(iter1));
}

ListIterator list_begin(List* self) {
	if (self && self->front)
		return (ListIterator) {
			NULL,
			self->front,
			self->front->next,
			0
		};
    else
        return ListIteratorNil;
}

ListIterator list_end(List* self) {
	if (self && self->back)
		return (ListIterator) {
			self->back,
			NULL,
			NULL,
			self->size
		};
	else
		return ListIteratorNil;
}


int list_push_front(List* self, ListNode* node) {
	if (!self || !node)
		return 0;

	if (self->front) {
		// connect the node
		node->prev = NULL;
		node->next = self->front;

		// set the list
		self->front->prev = node;
		self->front = node;
	}
	else {
		node->prev = node->next = NULL;
		self->front = self->back = node;
	}

	self->size += 1;
	return LIST_OK;
}

int list_push_back(List* self, ListNode* node) {
	if (!self || !node)
		return 0;

	if (self->front) {
		// connect the node
		node->next = NULL;
		node->prev = self->back;

		// set the list
		self->back->next = node;
		self->back = node;
	}
	else {
		node->prev = node->next = NULL;
		self->front = self->back = node;
	}

	self->size += 1;
	return LIST_OK;
}

int list_pop_front(List* self) {
	if (!self || !self->front)
		return 0;

	ListNode* pop_node = self->front;

	// set the list
	self->front = pop_node->next;
	if (self->front)
		self->front->prev = NULL;
	self->size -= 1;
	if (!self->size)
		self->back = NULL;

	// unlink the node
	pop_node->prev = pop_node->next = NULL;

	return LIST_OK;
}

int list_pop_back(List* self) {
	if (!self || !self->back)
		return 0;

	ListNode* pop_node = self->back;

	// set the list
	self->back = pop_node->prev;
	if (self->back)
		self->back->next = NULL;
	self->size -= 1;
	if (!self->size)
		self->front = NULL;

	// unlink the node
	pop_node->prev = pop_node->next = NULL;

	return LIST_OK;
}


ListIterator list_insert(List* self, ListIterator iter, ListNode* node) {
	if (!self || !node || !list_iterator_nil(iter))
		return ListIteratorNil;

	if (iter.node == self->front) {
		list_push_front(self, node);
		return list_begin(self);
	}
	else if (iter.prev == self->back) {
		list_push_back (self, node);
		return list_iterator_advance(list_end(self), -1);
	}

	// connect the node
	node->prev = iter.prev;
	node->next = iter.node;

	// set the neighbors
	node->prev->next = node;
	iter.node->prev = node;
	self->size += 1;

	// set rvalue
	ListIterator ret_iter = (ListIterator) {
		node->prev,
		node,
		node->next,
		iter.pos
	};

	return ret_iter;
}

ListIterator list_erase(List* self, ListIterator iter) {
	if (!self || !list_iterator_nil(iter) || iter.prev == self->back)
		return ListIteratorNil;

	if (iter.node == self->front)
	{
		list_pop_front(self);
		return list_begin(self);
	}
	else if (iter.node == self->back)
	{
		list_pop_back(self);
		return list_end(self);
	}

	// set the neighbors and the list
	if (iter.prev)
		iter.prev->next = iter.next;
	if (iter.next)
		iter.next->prev = iter.prev;
	self->size -= 1;

	// set rvalue
	ListIterator next_iter = (ListIterator) {
		iter.prev,
		iter.next,
		iter.next ? iter.next->next : NULL,
		iter.pos
	};

	// unlink the node
	iter.node->prev = iter.node->next = NULL;

	return next_iter;
}

size_t list_size(List* self) {
	if (self)
		return self->size;
	else
		return 0;
}
