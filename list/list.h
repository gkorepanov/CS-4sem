/* 
 * Doubly linked generic list implementation
 *
 ** Any function returns NULL or 0 in case of a failure and
 ** non-zero value otherwise.
 *
 ** Interface functions intentionally named consistently with std::list.
 */

#ifndef LIST_H
#define LIST_H

#include <stdlib.h>
#include <stddef.h>


// conscientiously borrowed from Linux kernel
#define list_entry(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

#define LIST_OK  0xE2E4 // for chess fans


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

List list_create  ();


/*****************************************************************************
 *                            ITERATOR DECLARATIONS                          *
 *****************************************************************************/

typedef ListNode* ListIterator;

// step iterator
ListIterator list_iterator_next (ListIterator iter);
ListIterator list_iterator_prev (ListIterator iter);

// acquire iterator pointing to first (last) element
ListIterator list_begin (List* self);
ListIterator list_end   (List* self);


/*****************************************************************************
 *                            	     METHODS                                 *
 *****************************************************************************/

// append the given node to the list
int list_push_front (List* self, ListNode* node);
int list_push_back  (List* self, ListNode* node);

// remove the first (last) element
int list_pop_front  (List* self);
int list_pop_back   (List* self);

// insert/remove element before/at the arbitrary place
int list_insert     (List* self, ListIterator iter, ListNode* node);
int list_erase      (List* self, ListIterator iter);

// store the list size in the corresponding variable
int list_size       (List* self, size_t* size);


#endif //LIST_H
