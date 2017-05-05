/* 
 * Doubly linked generic list implementation
 *
 ** Any function returns NULL, 0 or ListIteratorNil in case of failure
 ** and non-zero value otherwise.
 *
 ** Interface functions intentionally named consistently with std::list.
 */

#ifndef LIST_H
#define LIST_H

#include <stdlib.h>
#include <stddef.h>

#define LIST_OK  0xE2E4 // for chess fans

#define __list_entry(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

#define list_entry(iter, type, member) \
    __list_entry(iter.node, type, member)

/*****************************************************************************
 *                               MAIN STRUCTURES                             *
 *****************************************************************************/

typedef struct ListNode {
    struct ListNode *prev, *next;
} ListNode;

typedef struct {
    ListNode *front, *back;
    size_t size;
} List;

List list_create();


/*****************************************************************************
 *                            ITERATOR DECLARATIONS                          *
 *****************************************************************************/

typedef struct ListIterator {
    ListNode* prev;
    ListNode* node;
    ListNode* next;
    size_t pos;
} ListIterator;

const ListIterator ListIteratorNil;

// check if nil
int list_iterator_nil (ListIterator iter);

// step iterator
ListIterator list_iterator_next    (ListIterator* iter);
ListIterator list_iterator_prev    (ListIterator* iter);
ListIterator list_iterator_advance (ListIterator  iter, int step);

// acquire iterator pointing to first (past-the-end) element
ListIterator list_begin (List* self);
ListIterator list_end   (List* self);

/* compare iterators
 * > 0 : iter1 < iter2    forward
 * = 0 : iter1 = iter2
 * < 0 : iter1 > iter2    backward
 */
int   list_iterator_cmp (ListIterator iter1, ListIterator iter2);
void  list_for_each     (ListIterator iter1, ListIterator iter2,
                         void (*func)(void*));


/*****************************************************************************
 *                                     METHODS                                 *
 *****************************************************************************/

// append the given node to the list
int list_push_front (List* self, ListNode* node);
int list_push_back  (List* self, ListNode* node);

// remove the first (last) element
int list_pop_front  (List* self);
int list_pop_back   (List* self);

// insert/remove element before/at the arbitrary place
ListIterator list_insert (List* self, ListIterator iter, ListNode* node);
ListIterator list_erase  (List* self, ListIterator iter);

// store the list size in the corresponding variable
size_t list_size    (List* self);


#endif //LIST_H
