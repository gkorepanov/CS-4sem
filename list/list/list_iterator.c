#include "list.h"
#include "_list.h"

/*****************************************************************************
 *                         LIST ITERATOR FUNCTIONS                           *
 *****************************************************************************/

// NULL on failure
ListIterator *
list_iterator_new(List *list, ListDirection direction) {
    ListNode *node = direction == LIST_HEAD
      ? list->head
      : list->tail;

    return list_iterator_new_from_node(node, direction);
}


// NULL on failure.
ListIterator *
list_iterator_new_from_node(ListNode *node, ListDirection direction) {
    ListIterator *self;

    if (!(self = LIST_MALLOC(sizeof(ListIterator))))
        return NULL;

    self->next = node;
    self->direction = direction;

    return self;
}

// If there are no more nodes in the list, returns NULL
ListNode *
list_iterator_next(ListIterator *self) {
    ListNode *cur = self->next;

    if (cur) {
        self->next = self->direction == LIST_HEAD
          ? cur->next
          : cur->prev;
    }

    return cur;
}

void
list_iterator_destroy(ListIterator *self) {
    LIST_FREE(self);
    self = NULL;
}
