#ifndef LRU_LIST_H
#define LRU_LIST_H

#include <list>
#include <vector>

class LRUList {
    public:
        LRUList(int associativity);

        int get_lru() const;
        void make_mru(int way);

    private:
        std::list<int> m_list {};
        std::vector<std::list<int>::iterator> m_way_map {};

};

#endif