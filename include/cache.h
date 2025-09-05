#ifndef CACHE_H
#define CACHE_H

#include <string>
#include <vector>

#include <stdbool.h>
#include <stdint.h>

#include "binary-tree.h"
#include "lru_list.h"

#define CACHE_LINE_SIZE 64 // in bytes, non-powers of 2 will waste bits
#define CACHE_LINE_LOG2 findPowerOf2(CACHE_LINE_SIZE)

enum Policy { RANDOM, PLRU, LRU };

struct DirectoryEntry
{
    uint64_t tag { 0 };
    bool valid { false };
};

class Cache {
    public:
        Cache(Policy policy, int capacity, int associativity);

        bool is_cache_hit(int set, uint64_t tag);
        int select_victim_way(int set);
        void directory_write(int set, int way, uint64_t tag);

        int get_number_of_cache_sets() const;
    private:
        std::vector<std::vector<DirectoryEntry>> m_directory {};
        std::vector<Btree> m_plru_trees {};
        std::vector<LRUList> m_lru_lists {};

        enum Policy m_policy {};
        int m_capacity {};
        int m_associativity {};

        int m_number_of_cache_sets {};

        void init_cache_sets();

        int find_invalid_line(int set) const;
        int random_replacement(int set) const;
        int plru_replacement(int set);
        int lru_replacement(int set);
        void plru_update_on_invalid(int set, int way);
};

Policy cache_convert_policy(std::string_view policy_string);

// int cache_create(cache_t* cache, enum Policy policy, int capacity, int associativity);
// int cache_cleanup(cache_t* cache);

// directory_entry_t* cache_directory_read(const cache_t* cache, int set);
// void cache_directory_write(cache_t* cache, int set, int way, uint64_t tag);

// int cache_select_victim_way(const cache_t* cache, int set, int associativity);

// bool is_cache_hit(const cache_t* cache, int set, uint64_t tag);
// int find_invalid_line(const cache_t* cache, int set, int associativity);
// int random_replacement(const cache_t* cache, int set, int associativity);
// int plru_replacement(const cache_t* cache, int set, int associativity);
// int lru_replacement(const cache_t* cache, int set, int associativity);

// int plru_update_on_invalid(const cache_t* cache, int set, int way, int associativity);

#endif