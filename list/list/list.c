#include "list.h"
#include "_list.h"

/*****************************************************************************
 *                                   MEMORY                                  *
 *****************************************************************************/

void* my_malloc(size_t size) {
    int err;
    err = (rand() > (RAND_MAX/4)) ? 1 : 0;

    return err ? NULL : malloc(size);
}

/*****************************************************************************
 *                               LIST FUNCTIONS                              *
 *****************************************************************************/

// NULL on failure
ListNode *
list_node_new(void *val) {
    ListNode *self;

    if (!(self = LIST_MALLOC(sizeof(ListNode))))
        return NULL;

    self->prev = NULL;
    self->next = NULL;
    self->val = val;

    return self;
}

// NULL on failure
void *
list_node_getval(ListNode *node) {
    return node->val;
}


// NULL on failure
List *
list_new() {
    List *self;

    if (!(self = LIST_MALLOC(sizeof(List))))
      return NULL;

    self->head = NULL;
    self->tail = NULL;
    self->len = 0;

    return self;
}

void
list_node_destroy(ListNode* self) {
    LIST_FREE(self);
}


void
list_destroy(List *self) {
    unsigned int len = self->len;
    ListNode *next;
    ListNode *cur = self->head;

    while (len--) {
        next = cur->next;

        LIST_FREE(cur);
        cur = next;
    }

    LIST_FREE(self);
}


unsigned int
list_size(List *self) {
    return self->len;
}


// Append the node and return it
// NULL on failure
ListNode *
list_rpush(List *self, ListNode *node) {
  if (!node)
      return NULL;

  if (self->len) {
      node->prev = self->tail;
      node->next = NULL;
      self->tail->next = node;
      self->tail = node;
  } else {
      self->head = self->tail = node;
      node->prev = node->next = NULL;
  }

  ++self->len;
  return node;
}


// Detach last node from list
// NULL on empty list
ListNode *
list_rpop(List *self) {
    if (!self->len)
        return NULL;

    ListNode *node = self->tail;

    if (--self->len) {
        (self->tail = node->prev)->next = NULL;
    } else {
        self->tail = self->head = NULL;
    }

    node->next = node->prev = NULL;
    return node;
}


// Detach first node from list
// NULL on empty list
ListNode *
list_lpop(List *self) {
  if (!self->len)
      return NULL;

  ListNode *node = self->head;

  if (--self->len) {
      (self->head = node->next)->prev = NULL;
  } else {
      self->head = self->tail = NULL;
  }

  node->next = node->prev = NULL;
  return node;
}


// Prepend the node and return it
// NULL on failure
ListNode *
list_lpush(List *self, ListNode *node) {
  if (!node)
      return NULL;

  if (self->len) {
      node->next = self->head;
      node->prev = NULL;
      self->head->prev = node;
      self->head = node;
  } else {
      self->head = self->tail = node;
      node->prev = node->next = NULL;
  }

  ++self->len;
  return node;
}

// Return the node at the given index
// NULL on failure
ListNode *
list_at(List *self, int index) {
  ListDirection direction = LIST_HEAD;

  if (index < 0) {
    direction = LIST_TAIL;
    index = ~index;
  }

  if ((unsigned)index < self->len) {
      ListIterator *iter = list_iterator_new(self, direction);
      if (!iter)
          return NULL;
      ListNode* node = list_iterator_next(iter);
      
      while (index--)
          node = list_iterator_next(iter);

      list_iterator_destroy(iter);
      return node;
  }

  return NULL;
}


// Remove the node from the list
void
list_remove(List *self, ListNode *node) {
  node->prev
    ? (node->prev->next = node->next)
    : (self->head = node->next);

  node->next
    ? (node->next->prev = node->prev)
    : (self->tail = node->prev);

  LIST_FREE(node);
  --self->len;
}
