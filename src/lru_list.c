#include <stdlib.h>

#include "lru_list.h"

LRU_List_t* lru_list_init(int associativity) {
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
            lru_list->tail->next = temp;
            temp->prev = lru_list->tail;
            lru_list->tail = temp;
        }

        lru_list->way_map[i] = temp;
    }
    lru_list->tail->next = lru_list->head;

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

    // "pull" node from its current spot
    list_node->prev->next = list_node->next;
    list_node->next->prev = list_node->prev;

    // put node at front of list
    list_node->next = lru_list->head;
    list_node->prev = lru_list->tail;

    //update surrounding nodes
    list_node->next->prev = list_node;
    list_node->prev->next = list_node;

    // update head
    lru_list->head = list_node;
}