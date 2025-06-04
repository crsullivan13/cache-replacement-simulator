#ifndef CACHE_H
#define CACHE_H

#include <stdbool.h>
#include <stdint.h>

#include "binary-tree.h"
#include "lru_list.h"

#define CACHE_LINE_SIZE 64 // in bytes

enum Policy { RANDOM, PLRU, LRU };

typedef struct
{
    uint64_t tag;
    bool valid;
} directory_entry_t;

typedef struct
{
    uint64_t** data_array; // We aren't actually storing data, do we need this??
    directory_entry_t** directory;
    int** plru_trees;
    LRU_List_t** lru_lists;

    enum Policy policy;
    int capacity; // In KB, i.e. 1024 is 1 MB
    int associativity;

    int number_of_cache_sets;
} cache_t;

enum Policy cache_convert_policy(char* policy_string);

int cache_create(cache_t* cache, enum Policy policy, int capacity, int associativity);
int cache_cleanup(cache_t* cache);

directory_entry_t* cache_directory_read(const cache_t* cache, int set);
void cache_directory_write(cache_t* cache, int set, int way);

int cache_select_victim_way(const cache_t* cache, int set, int associativity);

int find_invalid_line(const cache_t* cache, int set, int associativity);
int random_replacement(const cache_t* cache, int set, int associativity);
int plru_replacement(const cache_t* cache, int set, int associativity);
int lru_replacement(const cache_t* cache, int set, int associativity);

int plru_update_on_invalid(const cache_t* cache, int set, int way, int associativity);

#endif