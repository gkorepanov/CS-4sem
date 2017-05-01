#include "list.h"

List list_create() {
	return (List){NULL, NULL, 0};
}

ListIterator list_iterator_next(ListIterator iter) {
	if (iter && iter->next)
		return iter->next;
	else
		return NULL;
}

ListIterator list_iterator_prev(ListIterator iter) {
	if (iter && iter->prev)
		return iter->prev;
	else
		return NULL;
}

ListIterator list_begin(List* self) {
	if (self && self->front)
		return self->front;
	else
		return NULL;
}

ListIterator list_end(List* self) {
	if (self && self->back)
		return self->back;
	else
		return NULL;
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

int list_insert(List* self, ListIterator iter, ListNode* node) {
	if (!self || !iter || !node)
		return 0;

	if (iter == self->front)
		return list_push_front(self, node);

	// connect the node
	node->prev = iter->prev;
	node->next = iter;

	// set the neighbors
	node->prev->next = node;
	node->next->prev = node;
	self->size += 1;

	return LIST_OK;
}

int list_erase(List* self, ListIterator iter) {
	if (!self || !iter)
		return 0;

	if (iter == self->front)
		return list_pop_front(self);
	else if (iter == self->back)
		return list_pop_back(self);

	// set the neighbors and the list
	iter->prev->next = iter->next;
	iter->next->prev = iter->prev;
	self->size -= 1;

	// unlink the node
	iter->prev = iter->next = NULL;
	
	return LIST_OK;
}

int list_size(List* self, size_t* size) {
	if (self && size) {
		*size = self->size;
		return LIST_OK;
	}
	else
		return 0;
}
