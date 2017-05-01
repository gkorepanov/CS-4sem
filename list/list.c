#include "list.h"

list_t list_create() {
	return (list_t){NULL, NULL, 0};
}

list_iterator_t list_iterator_next(list_iterator_t iter) {
	return (list_iterator_t){iter.node,
							 iter.next,
							 iter.next ? iter.next->next : NULL,
							 iter.pos + 1};
}

list_iterator_t list_iterator_prev(list_iterator_t iter) {
	return (list_iterator_t){iter.node,
						     iter.prev,
						     iter.prev ? iter.prev->prev : NULL,
						 	 iter.pos - 1};
}

list_iterator_t list_begin(list_t* self) {
	if(self && self->front)
		return (list_iterator_t){NULL,
		                         self->front,
		                         self->front->next,
		                         0};
	else
		return (list_iterator_t){NULL,
								 NULL,
								 NULL,
								 0};
}

list_iterator_t list_end(list_t* self) {
	return (list_iterator_t){(self && self->back) ? self->back : NULL,
		                     NULL,
		                     NULL,
		                     self.size};
}

int list_push_front(list_t* self, list_node_t* node) {
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

int list_push_back(list_t* self, list_node_t* node) {
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

int list_pop_front(list_t* self) {
	if (!self || !self->front)
		return 0;

	list_node_t* pop_node = self->front;

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

int list_pop_back(list_t* self) {
	if (!self || !self->back)
		return 0;

	list_node_t* pop_node = self->back;

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

int list_insert(list_t* self, list_iterator_t iter, list_node_t* node) {
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

int list_erase(list_t* self, list_iterator_t iter) {
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

int list_size(list_t* self, size_t* size) {
	if (self && size) {
		*size = self->size;
		return LIST_OK;
	}
	else
		return 0;
}
