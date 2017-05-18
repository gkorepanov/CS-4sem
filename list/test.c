#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "list/list.h"
#include "../tools/alerts.h"
#include <time.h>

#define N 10000
#define N_CUT 5000
#define N_PRINT 10

// Help macros to retry till there is no allocation error

#define REPEAT(...) \
    while(!(__VA_ARGS__)); /*{ \
        PRINTLN("Expression \"" RED #__VA_ARGS__ DCOLOR "\" returned error. Retrying..." ); \
    }*/

// Help function specific for list_at()
ListNode*
_list_at(List* list, unsigned int index) {
    ListNode* temp = NULL;

    REPEAT(temp = list_at(list, index));

    return temp;
}

// Help macros for pretty-printing
#define START_TEST(str) \
{\
  PRINT(GREEN str DCOLOR); \
  PRINT("%*s", (int)(60-strlen(str)), "");\
}\

#define TEST_OK() \
{\
  PRINT(GREEN "[OK]\n" DCOLOR);\
}



void
init_values(int *values) {
    for (unsigned int i = 1; i <= 2*N; ++i)
        values[i] = i;
}

void
create_list(List **list) {
    START_TEST("Creating list...");
    // Check allocation errors
    REPEAT(*list = list_new());

    assert(list_size(*list) == 0);

    TEST_OK();
}

void print_list(List *list) {
    PRINTLN(GREEN "Printing the list..." DCOLOR);
    ListIterator *iter = NULL;
    REPEAT(iter = list_iterator_new(list, LIST_TAIL));
     
    ListNode *node = NULL;
    unsigned int i = 0;
    while ((node = list_iterator_next(iter))) {
        PRINT("%d ", *(int*)list_node_getval(node));
        ++i;
        if (i > N_PRINT) {
            PRINT("...");
            break;
        }
    }
    PRINT("\n");

    list_iterator_destroy(iter);
}

void
check_integrity(List *list) {
    START_TEST("Checking integrity...");
    ListIterator *iter = NULL;
    REPEAT(iter = list_iterator_new(list, LIST_TAIL));

    ListNode *node = NULL, *temp = NULL;

    // iterating to end
    unsigned int i = 0;
    while ((temp = list_iterator_next(iter))) {
        node = temp;
        ++i;
    }

    // check that size equals
    assert(i == list_size(list));

    // check no more elements are there
    assert(list_iterator_next(iter) == NULL);

    // check last node is the one we've got to
    assert(node == list_lpop(list));
    list_rpush(list, node);

    assert(node == _list_at(list, list_size(list) - 1));

    assert(node == _list_at(list, -1));

    // repeat for reverse iterator

    list_iterator_destroy(iter);

    REPEAT(iter = list_iterator_new(list, LIST_HEAD));

    // iterating to front
    i = 0;
    while ((temp = list_iterator_next(iter))) {
        node = temp;
        ++i;
    }

    // check that size equals
    assert(i == list_size(list));

    // check no more elements are there
    assert(list_iterator_next(iter) == NULL);

    // check last node is the one we've got to
    assert(node == list_rpop(list));
    list_lpush(list, node);
    assert(node == _list_at(list, 0));

    list_iterator_destroy(iter);

    TEST_OK();
}


void
fill_list(List *list, int *values) {
    START_TEST("Filling list...");
    unsigned int initial_size = list_size(list);

    ListNode* node;

    for (unsigned int i = 1; i <= N; ++i) {
        REPEAT(node = list_node_new(&values[i])); 
        list_rpush(list, node);

        REPEAT(node = list_node_new(&values[i]));
        list_lpush(list, node);

        //("List size: %u", list_size(list));
        //("Expected size: %u", initial_size + i*2);

        assert(list_size(list) == (initial_size + i*2));
    }

    TEST_OK();
}


void
cut_list(List *list) {
    START_TEST("Cutting the list down...");

    unsigned int initial_size = list_size(list);

    for (unsigned int i = 1; i <= N_CUT; ++i) {
        list_node_destroy(list_rpop(list));
        list_node_destroy(list_lpop(list));

        assert(list_size(list) == (initial_size - (i*2)));
    }

    initial_size = list_size(list);
    list_remove(list, _list_at(list, 0));
    assert(list_size(list) == (initial_size - 1));

    initial_size = list_size(list);
    list_remove(list, _list_at(list, -1));
    assert(list_size(list) == (initial_size - 1));

    TEST_OK();
}

void check_failures(List* list) {
    START_TEST("Going through failure ways...");

    assert(list_rpush(list, NULL) == NULL);
    assert(list_lpush(list, NULL) == NULL);
    assert(list_at(list, list_size(list)) == NULL);


    REPEAT(list = list_new());

    assert(list_rpop(list) == NULL);
    assert(list_lpop(list) == NULL);

    int a  = 237;

    ListNode *node = NULL;

    // Check inserts and pops and removes for last elements
    REPEAT(node = list_node_new(&a));
    list_rpush(list, node);
    node = list_rpop(list);
    assert(*(int*)list_node_getval(node) == 237);
    list_node_destroy(node);

    REPEAT(node = list_node_new(&a));
    list_lpush(list, node);
    node = list_lpop(list);
    assert(*(int*)list_node_getval(node) == 237);
    list_node_destroy(node);

    REPEAT(node = list_node_new(&a));
    list_rpush(list, node);
    list_remove(list, _list_at(list, 0));

    assert(list_size(list) == 0);
    assert(list_rpop(list) == NULL);

    list_destroy(list);

    TEST_OK();
}


int
main() {

#ifdef DEBUG
    srand(time(NULL)); // for allocation errors to arise randomly
#endif

    int *values = calloc(2*N+1, sizeof(int));

    List *list = NULL;

    init_values(values);

    create_list(&list);
    fill_list(list, values);
    print_list(list);
    check_integrity(list);
    cut_list(list);
    print_list(list);
    fill_list(list, values);
    check_failures(list);
    list_destroy(list);
    free(values);

    PRINT(GREEN
            "-------------------------------------------------------------\n"
            "                     EVERY TEST PASSED                       \n"
            "-------------------------------------------------------------\n"
          DCOLOR);
    return 0;
}
