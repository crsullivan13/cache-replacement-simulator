#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "cache.h"

// Takes user input string and converts to policy enum value
// Defaults to RANDOM if policy given is not valid
enum Policy cache_convert_policy(char* policy_string) {
    enum Policy policy = RANDOM;

    if ( strncmp("random", policy_string, strlen("random")) == 0 ) {
        policy = RANDOM;
    } else if ( strncmp("plru", policy_string, strlen("plru")) == 0 ) {
        policy = PLRU;
    } else if ( strncmp("lru", policy_string, strlen("lru")) == 0 ) {
        policy = LRU;
    }

    return policy;
}

void cleanup_sets_on_init_error(cache_t* cache, enum Policy policy, int allocated_sets) {
    if ( allocated_sets == 0 ) {

    } else {
        for ( int i = 0; i < allocated_sets; i++ ) {
            free(cache->data_array[i]);
            free(cache->directory[i]);

            if ( cache->policy == PLRU ) {
                // TODO
            } else if ( cache->policy == LRU ) {
                // TODO
            }
        }
    }
}

int init_cache_sets(cache_t* cache, enum Policy policy, int number_of_cache_sets) {
    int error = 0;
    int i = 0;

    for ( i = 0; i < number_of_cache_sets; i++ ) {
        cache->data_array[i] = malloc(sizeof(uint64_t) * ( CACHE_LINE_SIZE / sizeof(uint64_t) ) * cache->associativity);
        if ( cache->data_array[i] == NULL ) {
            error = 1;
            fprintf(stderr, "ERROR: Cache data array init for set %d failed due to NULL malloc return\n", i);
            break;
        }

        cache->directory[i] = malloc(sizeof(directory_entry_t) * cache->associativity);
        if ( cache->directory[i] == NULL ) {
            error = 1;
            fprintf(stderr, "ERROR: Cache directory init for set %d failed due to NULL malloc return\n", i);
            break;
        } else {
            memset(cache->directory[i], 0, sizeof(directory_entry_t) * cache->associativity);
        } 

        if ( cache->policy == PLRU ) {
            cache->plru_trees[i] = btree_init(cache->associativity - 1);
            if ( cache->plru_trees[i] == NULL ) {
                error = 1;
                fprintf(stderr, "ERROR: Cache init for set %d failed due to NULL plru tree root\n", i);
                break;
            } else {
                memset(cache->plru_trees[i], 0, sizeof(int) * cache->associativity);
            }
        } else if ( cache->policy == LRU ) {
            cache->lru_lists[i] = lru_list_init(cache->associativity);
            if ( cache->lru_lists[i] == NULL ) {
                error = 1;
                fprintf(stderr, "ERROR: Cache init for set %d failed due to NULL lru list\n", i);
                break;
            }
        }
    }

    if ( error == 1 ) {
        cleanup_sets_on_init_error(cache, policy, i);
    }

    return error;
}

// creates and inits our cache model
//TODO: Handle errors
int cache_create(cache_t* cache, enum Policy policy, int capacity, int associativity) {
    int error = 0;

    cache->policy = policy;
    cache->capacity = capacity;
    cache->associativity = associativity;

    int cache_set_size = associativity * CACHE_LINE_SIZE;
    int number_of_cache_sets = (capacity * 1024) / cache_set_size;
    //printf("Set size %d, number of sets %d\n", cache_set_size, number_of_cache_sets);
    cache->number_of_cache_sets = number_of_cache_sets;

    cache->data_array = malloc(sizeof(uint64_t*) * number_of_cache_sets);
    if ( cache->data_array == NULL ) {
        error = 1;
        fprintf(stderr, "ERROR: Cache init failed due data array NULL malloc return\n");
        return error;
    }

    cache->directory = malloc(sizeof(directory_entry_t*) * number_of_cache_sets);
    if ( cache->directory == NULL ) {
        error = 1;
        fprintf(stderr, "ERROR: Cache init failed due directory NULL malloc return\n");
        return error;
    }

    if ( cache->policy == PLRU ) {
        cache->plru_trees = malloc(sizeof(int*) * number_of_cache_sets);
        if ( cache->plru_trees == NULL ) {
            error = 1;
            fprintf(stderr, "ERROR: Cache init failed due prlu trees NULL malloc return\n");
            return error;
        }
    } else {
        cache->plru_trees = NULL;
    }

    if ( cache->policy == LRU ) {
        cache->lru_lists = malloc(sizeof(LRU_List_t*) * number_of_cache_sets);
        if ( cache->lru_lists == NULL ) {
            error = 1;
            fprintf(stderr, "ERROR: Cache init failed due lru lists NULL malloc return\n");
            return error;
        }
    } else {
        cache->lru_lists = NULL;
    }

    if ( error == 0) {
        error = init_cache_sets(cache, policy, number_of_cache_sets);
    }

    return error;
}

int cache_cleanup(cache_t* cache) {
    for ( int i = 0; i < cache->number_of_cache_sets; i++ ) {
        free(cache->data_array[i]);
        free(cache->directory[i]);

        if ( cache->policy == PLRU ) {
            btree_cleanup(cache->plru_trees[i]);
        } else if ( cache->policy == LRU ) {
            lru_list_cleanup(cache->lru_lists[i], cache->associativity);
        }
    }

    free(cache->data_array);
    free(cache->directory);
    if ( cache->policy == PLRU ) {
        free(cache->plru_trees);
    } else if ( cache->policy == LRU) {
        free(cache->lru_lists);
    }

    return 0;
}

directory_entry_t* cache_directory_read(const cache_t* cache, int set) {
    return cache->directory[set];
}

// TODO: make this actually useful
void cache_directory_write(cache_t* cache, int set, int way) {
    cache->directory[set][way].valid = true;
}

int cache_select_victim_way(const cache_t* cache, int set, int associativity) {
    int victim_way = find_invalid_line(cache, set, associativity);

    // negtaive means no invalid lines in the set and we need to call a replacement policy
    // if not negative (i.e. we found and empty line) we may need to do some other tasks to satisfy the policy
    if ( victim_way == -1 ) {
        switch (cache->policy)
        {
        case RANDOM:
            victim_way = random_replacement(cache, set, associativity);
            break;
        case PLRU:
            victim_way = plru_replacement(cache, set, associativity);
            break;
        case LRU:
            victim_way = lru_replacement(cache, set, associativity);
            break;
        default:
            victim_way = random_replacement(cache, set, associativity);
            break;
        }
    } else {
        // be explicit about what cases don't need anything
        switch (cache->policy)
        {
        case RANDOM:
            // Nothing to be done
            break;
        case PLRU:
            //printf("PLRU: Update on invalid\n");
            plru_update_on_invalid(cache, set, victim_way, associativity);
            break;
        case LRU:
            //printf("LRU: Update on invalid\n");
            lru_update_on_invalid(cache->lru_lists[set], victim_way);
            break;
        default:
            // default is RANDOM so do nothing
            break;
        }
    }

    return victim_way;
}

int find_invalid_line(const cache_t* cache, int set, int associativity) {
    int victim_way = -1;
    directory_entry_t* directory_data = cache_directory_read(cache, set);

    // Select invalid lines first
    for ( int i = 0; i < associativity; i++ ) {
        if ( directory_data[i].valid == false ) {
            victim_way = i;
            //printf("EMPTY: Invalid (empty) way selected is %d\n", victim_way);
            break;
        }
    }

    return victim_way;
}

int random_replacement(const cache_t* cache, int set, int associativity) {
    int victim_way = 0;
    directory_entry_t* directory_data = cache_directory_read(cache, set);

    int lfsr_value = rand() % 1024; // Following the way this is done in rocket-chip-inclusive-cache code
                                    // i.e. not real lfsr, but simulated

    for ( int i = 0; i < associativity; i++ ) {
        if ( ( ( 1024 / associativity ) * i ) <= lfsr_value ) {
            victim_way = i;
        }
    }
    //printf("RANDOM: No invalids, random selection is %d for lfsr value %d\n", victim_way, lfsr_value);

    return victim_way;
}

// 0 -> right is lru
// 1 -> left is lru
int plru_replacement(const cache_t* cache, int set, int associativity) {
    int victim_way = 0;
    int* plru_tree = cache->plru_trees[set];

    // traverse actual nodes to find leaf that is lru
    int next_index = 0;
    while ( next_index <= ( associativity - 2 ) ) {
        victim_way = next_index;
        if ( plru_tree[next_index] == 0 ) {
            plru_tree[next_index] = 1;
            next_index = btree_get_right_child(next_index);
        } else {
            plru_tree[next_index] = 0;
            next_index = btree_get_left_child(next_index);
        }
    }

    // we only have associativity - 1 nodes in the tree, but we need to get the cache way that is lru
    // go one level past lru leaf in tree to get the lru "node", then do math to convert node index to cache way
    victim_way = plru_tree[victim_way] == 0 ? btree_get_right_child(victim_way) : btree_get_left_child(victim_way);
    victim_way = victim_way - ( associativity - 1);
    //printf("PLRU: Selected way is %d\n", victim_way);

    return victim_way;
}

int lru_replacement(const cache_t* cache, int set, int associativity) {
    int victim_way = 0;
    LRU_List_t* lru_list = cache->lru_lists[set];

    victim_way = lru_list_get_tail(lru_list)->way_index;
    lru_list_move_to_head(lru_list, victim_way);
    //printf("LRU: Selected way is %d\n", victim_way);

    return victim_way;
}

// Update the tree since we just accessed a line
// 0 -> right is lru
// 1 -> left is lru
int plru_update_on_invalid(const cache_t* cache, int set, int way, int associativity) {
    int* plru_tree = cache->plru_trees[set];
    int old_parent_index = way + ( associativity - 1 );
    int new_parent_index = old_parent_index;

    while ( new_parent_index != 0 )
    {
        new_parent_index = btree_get_parent(new_parent_index);
        plru_tree[new_parent_index] = old_parent_index % 2 ? 0 : 1;
        old_parent_index = new_parent_index;
    }

    return 0;
}