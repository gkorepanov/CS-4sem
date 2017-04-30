/* 
 * list.h -- the simple library implementing double linked generic list
 */

#ifndef LIST_H
#define LIST_H

/*
 * for functions returning the list_iterator_t:
 * NULL means error
 */

/* 
 * for functions returning the int:
 * 0 means error
 */ 

// honestly stolen from Linux kernel
#define list_entry(ptr, type, member) \
    ((type *)((char *)(ptr) – offsetof(type, member)))

typedef int bool;

/*
 *******************************************************************************
 *                               MAIN STRUCTURES                               *
 *******************************************************************************
 */
typedef struct node_t {
	struct node_t* prev, next;
} list_node_t;

typedef struct {
	node_t* front, back;
	size_t size;
} list_t;

list_t list_create();

int    list_destroy(list_t* self);
/*
 *******************************************************************************
 *                            ITERATOR DECLARATIONS                            *
 *******************************************************************************
 */
typedef node_t* list_iterator_t;

// step iterator
list_iterator_t list_iterator_next    (list_iterator_t* iter);
list_iterator_t list_iterator_prev    (list_iterator_t* iter);

// acquire iterator pointing to first (last) element
list_iterator_t list_begin (list_t* self);
list_iterator_t list_end   (list_t* self);


/*
 *******************************************************************************
 *                            	     METHODS                                   *
 *******************************************************************************
 */
// append the given node to the list
int list_push_front    (list_t* self, node_t* node);
int list_push_back     (list_t* self, node_t* node);

// remove the first (last) element
int list_pop_front     (list_t* self);
int list_pop_back      (list_t* self);

// insert new element at the arbitrary place
int list_insert        (list_t* self, list_iterator_t* iter, node_t* node);

int list_remove        (list_t* self, list_iterator_t* iter);

int list_is_empty      (list_t* self);

// stores the size of the list in size variable
int list_size( list_t* self, size_t* size);


#endif
