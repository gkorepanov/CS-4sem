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

typedef struct list_node_t {
	struct list_node_t *prev, *next;
} list_node_t;

typedef struct {
	list_node_t *front, *back;
	size_t size;
} list_t;

list_t list_create();


/*****************************************************************************
 *                            ITERATOR DECLARATIONS                          *
 *****************************************************************************/

typedef struct list_iterator_t {
	__list_node_t* node;
	size_t pos;
} list_iterator_t;

// step iterator
list_iterator_t list_iterator_next (list_iterator_t iter);
list_iterator_t list_iterator_prev (list_iterator_t iter);

// acquire iterator pointing to first (past-the-end) element
list_iterator_t list_begin (list_t* self);
list_iterator_t list_end   (list_t* self);

/* compare iterators
 * <0 : iter1 < iter2;
 * =0 : iter1 = iter2;
 * >0 : iter1 > iter2 (consistent with strcmp) 
 */
int list_iterator_cmp      (list_iterator_t iter1, list_iterator_t iter2);


/*****************************************************************************
 *                            	     METHODS                                 *
 *****************************************************************************/

// append the given node to the list
int list_push_front (list_t* self, list_node_t* node);
int list_push_back  (list_t* self, list_node_t* node);

// remove the first (last) element
int list_pop_front  (list_t* self);
int list_pop_back   (list_t* self);

// insert/remove element before/at the arbitrary place
int list_insert     (list_t* self, list_iterator_t iter, list_node_t* node);
int list_erase      (list_t* self, list_iterator_t iter);

// store the list size in the corresponding variable
int list_size       (list_t* self, size_t* size);


#endif //LIST_H
