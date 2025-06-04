#include <stdlib.h>
#include <math.h>

#include "binary-tree.h"

int* btree_init(int number_of_nodes) {
    return malloc(sizeof(int) * number_of_nodes);
}

void btree_cleanup(int* tree) {
    free(tree);
}

int btree_get_left_child(int index) {
    return index * 2 + 1;
}

int btree_get_right_child(int index) {
    return index * 2 + 2;
}

int btree_get_parent(int index) {
    if ( index <= 0 ) {
        return 0; // shouldn't ever happen in our use case
    } else {
        return floor( ( index -1 ) / 2 );
    }
}