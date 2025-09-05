#include <stdlib.h>
#include <math.h>

#include "binary-tree.h"

int Btree::get_left_child(int index) const {
    return index * 2 + 1;
}

int Btree::get_right_child(int index) const {
    return index * 2 + 2;
}

int Btree::get_parent(int index) const {
    if ( index <= 0 ) {
        return 0;
    } else {
        return floor( ( index - 1 ) / 2 );
    }
}

void Btree::set_node_value(int index, int value) {
    m_tree[index] = value;
}

int Btree::get_node_value(int index) const {
    return m_tree[index];
}