#ifndef BINARY_TREE_H
#define BINARY_TREE_H

#include <memory>

// very simple array based binary tree implementation, for plru

class Btree {
    public:
        Btree(int number_of_nodes)
            : m_tree { std::make_unique<int[]>(number_of_nodes) } {}

        int get_left_child(int index) const;
        int get_right_child(int index) const;
        int get_parent(int index) const;

        void set_node_value(int index, int value);
        int get_node_value(int index) const;

    private:
        std::unique_ptr<int[]> m_tree {};
};

#endif