#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <string>
#include <iostream>

#include "cache.h"

static constexpr int compute_sets(int capacity, int associativity) {
    return ( capacity * 1024 ) / ( associativity * CACHE_LINE_SIZE );
}

// Takes user input string and converts to policy enum value
// Defaults to RANDOM if policy given is not valid
Policy cache_convert_policy(std::string_view policy_string) {
    Policy policy = RANDOM;

    if ( "random" == policy_string ) {
        policy = RANDOM;
    } else if ( "plru" == policy_string ) {
        policy = PLRU;
    } else if ( "lru" == policy_string ) {
        policy = LRU;
    }

    return policy;
}

void Cache::init_cache_sets() {
    if ( m_policy == PLRU ) {
        m_plru_trees.reserve(m_number_of_cache_sets);
    } 

    for ( int i = 0; i < m_number_of_cache_sets; i++ ) {
        m_directory.emplace_back(m_associativity);

        if ( m_policy == PLRU ) {
            m_plru_trees.emplace_back(m_associativity - 1);
        } else if ( m_policy == LRU ) {
           m_lru_lists.emplace_back(m_associativity);
        }
    }
}

Cache::Cache(Policy policy, int capacity, int associativity) 
            : m_policy { policy }, 
            m_capacity { capacity }, 
            m_associativity { associativity },
            m_number_of_cache_sets { compute_sets(capacity, associativity) }
{
    init_cache_sets();
}


bool Cache::is_cache_hit(int set, uint64_t tag) {
    bool is_hit = false;

    const std::vector<DirectoryEntry>& directory_data = m_directory[set];
    int i;
    for ( i = 0; i < m_associativity; i++ ) {
        if ( directory_data[i].tag == tag && directory_data[i].valid ){
            is_hit = true;
            break;
        }
    }

    // TODO: Move this somewhere else, detach hit checking with metadata updates
    if ( is_hit ) {
        if ( m_policy == PLRU ) {
            plru_update_on_invalid(set, /*way=*/i);
        } else if ( m_policy == LRU ) {
            m_lru_lists[set].make_mru(i);
        }
    }

    return is_hit;
}

int Cache::find_invalid_line(int set) const {
    int victim_way = -1;
    const std::vector<DirectoryEntry>& directory_data = m_directory[set];

    // Select invalid lines first
    for ( int i = 0; i < m_associativity; i++ ) {
        if ( directory_data[i].valid == false ) {
            victim_way = i;
            //printf("EMPTY: Invalid (empty) way selected is %d\n", victim_way);
            break;
        }
    }

    return victim_way;
}

int Cache::random_replacement(int set) const {
    int victim_way = 0;

    int lfsr_value = rand() % 1024; // Following the way this is done in rocket-chip-inclusive-cache code
                                    // i.e. not real lfsr, but simulated

    for ( int i = 0; i < m_associativity; i++ ) {
        if ( ( ( 1024 / m_associativity ) * i ) <= lfsr_value ) {
            victim_way = i;
        }
    }
    //printf("RANDOM: No invalids, random selection is %d for lfsr value %d\n", victim_way, lfsr_value);

    return victim_way;
}

// 0 -> right is lru
// 1 -> left is lru
int Cache::plru_replacement(int set) {
    int victim_way = 0;
    Btree& plru_tree = m_plru_trees[set];

    // traverse actual nodes to find leaf that is lru
    int next_index = 0;
    while ( next_index <= ( m_associativity - 2 ) ) {
        victim_way = next_index;
        if ( plru_tree.get_node_value(next_index) == 0 ) {
            plru_tree.set_node_value(next_index, 1);
            next_index = plru_tree.get_right_child(next_index);
        } else {
            plru_tree.set_node_value(next_index, 0);
            next_index = plru_tree.get_left_child(next_index);
        }
    }

    // we only have associativity - 1 nodes in the tree, but we need to get the cache way that is lru
    // go one level past lru leaf in tree to get the lru "node", then do math to convert node index to cache way
    victim_way = plru_tree.get_node_value(victim_way) == 0 ?  plru_tree.get_right_child(victim_way) : plru_tree.get_left_child(victim_way);
    victim_way = victim_way - ( m_associativity - 1);
    //printf("PLRU: Selected way is %d\n", victim_way);

    return victim_way;
}

int Cache::lru_replacement(int set) {
    int victim_way = 0;
    LRUList& lru_list = m_lru_lists[set];

    victim_way =  lru_list.get_lru();
    lru_list.make_mru(victim_way);
    //printf("LRU: Selected way is %d\n", victim_way);

    return victim_way;
}

// Update the tree since we just accessed a line
// 0 -> right is lru
// 1 -> left is lru
void Cache::plru_update_on_invalid(int set, int way) {
    Btree& plru_tree = m_plru_trees[set];
    int old_parent_index = way + ( m_associativity - 1 );
    int new_parent_index = old_parent_index;

    while ( new_parent_index != 0 )
    {
        new_parent_index = plru_tree.get_parent(new_parent_index);
        plru_tree.set_node_value(new_parent_index, old_parent_index % 2 ? 0 : 1);
        old_parent_index = new_parent_index;
    }
}

int Cache::select_victim_way(int set) {
    int victim_way = find_invalid_line(set);
    std::cout << m_plru_trees[set].to_string() << "\n";

    // negtaive means no invalid lines in the set and we need to call a replacement policy
    // if not negative (i.e. we found and empty line) we may need to do some other tasks to satisfy the policy
    if ( victim_way == -1 ) {
        switch (m_policy)
        {
        case RANDOM:
            victim_way = random_replacement(set);
            break;
        case PLRU:
            victim_way = plru_replacement(set);
            break;
        case LRU:
            victim_way = lru_replacement(set);
            break;
        default:
            victim_way = random_replacement(set);
            break;
        }
    } else {
        switch (m_policy)
        {
        case RANDOM:
            // Nothing to be done
            break;
        case PLRU:
            //printf("PLRU: Update on invalid\n");
            plru_update_on_invalid(set, victim_way);
            break;
        case LRU:
            //printf("LRU: Update on invalid\n");
            m_lru_lists[set].make_mru(victim_way);
            break;
        default:
            // default is RANDOM so do nothing
            break;
        }
    }

    std::cout << "VICTIM WAY: " << victim_way << "\n";
    std::cout << m_plru_trees[set].to_string() << "\n";
    return victim_way;
}

void Cache::directory_write(int set, int way, uint64_t tag) {
    std::vector<DirectoryEntry>& directory_data = m_directory[set];
    directory_data[way].valid = true;
    directory_data[way].tag = tag;
}

int Cache::get_number_of_cache_sets() const {
    return m_number_of_cache_sets;
}