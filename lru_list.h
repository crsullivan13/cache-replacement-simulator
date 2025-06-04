#ifndef LRU_LIST_H
#define LRU_LIST_H

typedef struct Node_t {
    struct Node_t* next;
    struct Node_t* prev;

    int way_index; // could be uint8_t but doesn't really matter for the sim
} Node_t;

typedef struct
{
    Node_t* head;
    Node_t* tail;
    Node_t** way_map; // Map to quickly get way node on access rather than searching whole list
} LRU_List_t;

void lru_list_init(LRU_List_t* lru_list, int associativity);
void lru_list_cleanup(LRU_List_t* lru_list, int associativity);

Node_t* lru_list_get_tail(LRU_List_t* lru_list); // get lru
Node_t* lru_list_get_head(LRU_List_t* lru_list); // get mru
void lru_list_move_to_head(LRU_List_t* lru_list, int way); // move accessed way to head of list, i.e. mru

#endif