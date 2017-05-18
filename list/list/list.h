#ifndef LIST_H
#define LIST_H

#include <stdlib.h>

/*****************************************************************************
 *                            MAIN DECLARATIONS                              *
 *****************************************************************************/

struct ListNode;
struct List;
struct ListIterator;

typedef struct ListNode ListNode;
typedef struct List List;
typedef struct ListIterator ListIterator;

typedef enum {
    LIST_HEAD,
    LIST_TAIL
} ListDirection;


/*****************************************************************************
 *                               NODE FUNCTIONS                              *
 *****************************************************************************/

ListNode *
list_node_new(void *val);

void
list_node_destroy(ListNode* self);

void *
list_node_getval(ListNode *node);


/*****************************************************************************
 *                               LIST FUNCTIONS                              *
 *****************************************************************************/

List *
list_new();

ListNode *
list_rpush(List *self, ListNode *node);

ListNode *
list_lpush(List *self, ListNode *node);

ListNode *
list_at(List *self, int index);

ListNode *
list_rpop(List *self);

ListNode *
list_lpop(List *self);

unsigned int
list_size(List *self);

void
list_remove(List *self, ListNode *node);

void
list_destroy(List *self);


/*****************************************************************************
 *                           ITERATOR FUNCTIONS                              *
 *****************************************************************************/

ListIterator *
list_iterator_new(List *list, ListDirection direction);

ListIterator *
list_iterator_new_from_node(ListNode *node, ListDirection direction);

ListNode *
list_iterator_next(ListIterator *self);

void
list_iterator_destroy(ListIterator *self);

#endif /* LIST_H */
