#include <stdlib.h>
#include <math.h>

#include <iostream>

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

std::string get_spaces(int level) {
    std::string tabs;
    for(int i = 0; i < level - 1; i++){
        tabs += " ";
    }
    return tabs;
}

std::string Btree::to_string() {
    std::string str;
    int levels { static_cast<int>(log(m_num_nodes + 1)) + 1 };
    for(int i = 0; i < levels; i++) {
        str += get_spaces(levels - i);
        if ( i == 0 ) {
            str += std::to_string(m_tree[0]);
        } else {
            int base { static_cast<int>(pow(2,i)) };
            for(int j = 0; j < base; j++) {
                str = str + std::to_string(m_tree[j + (base - 1)]) + " ";
            }
        }
        str += "\n";
    }
    return str;
}