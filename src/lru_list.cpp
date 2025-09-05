#include "lru_list.h"

#include <iterator>

LRUList::LRUList(int associativity)
{
    for ( int i = 0; i < associativity; i++ ) {
        m_list.emplace_back(i);
        auto itr = std::prev(m_list.end());
        m_way_map.push_back(itr);
    }
}

int LRUList::get_lru() const {
    return m_list.back();
}

void LRUList::make_mru(int way) {
    auto itr = m_way_map[way];
    m_list.splice(m_list.begin(), m_list, itr);
}