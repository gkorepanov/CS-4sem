#ifndef _LIST_H
#define _LIST_H

#include <stdlib.h>

/*****************************************************************************
 *                                   MEMORY                                  *
 *****************************************************************************/

#ifndef LIST_MALLOC

#ifndef DEBUG
    #define LIST_MALLOC malloc
#else
    #define LIST_MALLOC my_malloc
#endif

#endif

#ifndef LIST_FREE
#define LIST_FREE free
#endif

void* my_malloc(size_t size); 

/*****************************************************************************
 *                        STRUCTURES DEFINITIONS                             *
 *****************************************************************************/

struct List {
    ListNode *head;
    ListNode *tail;
    unsigned int len;
    //void (*free)(void *val);
};

struct ListNode {
    ListNode *prev;
    ListNode *next;
    void *val;
};

struct ListIterator {
    ListNode *next;
    ListDirection direction;
};


#endif /* _LIST_H */
