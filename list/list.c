#include "list.h"

List list_create() {
	return (List) {
		NULL,
		NULL,
		0
	};
}

const ListIterator ListIteratorNil = (ListIterator) {
	NULL,
	NULL,
	NULL,
	0
};

ListIterator list_iterator_next(ListIterator* iter) {
	if (!iter)
		return ListIteratorNil;

	*iter = (ListIterator) {
		iter->node,
		iter->next,
		iter->next ? iter->next->next : NULL,
		iter->pos + 1
	};
	return *iter;
}

ListIterator list_iterator_prev(ListIterator* iter) {
	if (!iter)
		return ListIteratorNil;

	*iter = (ListIterator) {
		iter->node,
		iter->prev,
		iter->prev ? iter->prev->prev : NULL,
		iter->pos - 1
	};
	return *iter;
}

int list_iterator_cmp(ListIterator a, ListIterator b) {
	return a.pos - b.pos;
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

int list_insert(List* self, ListIterator iter, ListNode* node) {
	if (!self /*|| !iter*/ || !node)
		return 0;

	if (iter.node == self->front)
		return list_push_front(self, node);
	else if (iter.node == self->back)
		return list_push_back (self, node);

	// connect the node
	node->prev = iter.node->prev;
	node->next = iter.node;

	// set the neighbors
	node->prev->next = node;
	node->next->prev = node;
	self->size += 1;

	return LIST_OK;
}

int list_erase(List* self, ListIterator iter) {
	if (!self /*|| !iter*/)
		return 0;

	if (iter.node == self->front)
		return list_pop_front(self);
	else if (iter.node == self->back)
		return list_pop_back(self);

	// set the neighbors and the list
	iter.node->prev->next = iter.node->next;
	iter.node->next->prev = iter.node->prev;
	self->size -= 1;

	// unlink the node
	iter.node->prev = iter.node->next = NULL;
	
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
