#include <stdlib.h>
#include <stdio.h>

#include "lru_list.h"

// Initializes an LRU list for given associativity
// Returns NULL on error 
LRU_List_t* lru_list_init(int associativity) {
    if ( associativity <= 0 ) {
        return NULL;
    }

    LRU_List_t* lru_list = malloc(sizeof(LRU_List_t));
    lru_list->way_map = malloc(sizeof(Node_t*) * associativity);

    Node_t* temp = NULL;
    for ( int i = 0; i < associativity; i++ ) {
        temp = malloc(sizeof(Node_t));
        temp->next = NULL;
        temp->prev = NULL;
        temp->way_index = i;

        if ( i == 0 ) {
            lru_list->head = temp;
            lru_list->tail = temp;
            temp->next = temp;
            temp->prev = temp;
        } else {
            temp->prev = lru_list->tail;
            lru_list->tail->next = temp;
            lru_list->tail = temp;
        }

        lru_list->way_map[i] = temp;
    }
    lru_list->tail->next = lru_list->head;

    //printf("Tail is %d, head is %d\n", lru_list->tail->way_index, lru_list->head->way_index);

    return lru_list;
}

void lru_list_cleanup(LRU_List_t* lru_list, int associativity) {
    for ( int i = 0; i < associativity; i++ ) {
        free(lru_list->way_map[i]);
    }
    lru_list->head = NULL;
    lru_list->tail = NULL;
    free(lru_list->way_map);
    free(lru_list);
}

Node_t* lru_list_get_tail(LRU_List_t* lru_list) {
    return lru_list->tail;
}

Node_t* lru_list_get_head(LRU_List_t* lru_list) {
    return lru_list->head;
}

void lru_list_move_to_head(LRU_List_t* lru_list, int way) {
    Node_t* list_node = lru_list->way_map[way];

    // do need to do anything if way is already mru (at head of list)
    if ( lru_list->head != list_node ) {
        // "pull" node from its current spot
        list_node->prev->next = list_node->next;
        list_node->next->prev = list_node->prev;
        if ( lru_list->tail == list_node ) {
            lru_list->tail = list_node->prev;
        }

        // put node at front of list
        list_node->next = lru_list->head;
        list_node->prev = lru_list->tail;

        //update surrounding nodes
        list_node->next->prev = list_node;
        list_node->prev->next = list_node;

        // update head
        lru_list->head = list_node;
    }
}

void lru_update_on_invalid(LRU_List_t* lru_list, int victim_way) {
    lru_list_move_to_head(lru_list, victim_way);
    //printf("LRU: tail (lru) is now %d\n", lru_list->tail->way_index);
}