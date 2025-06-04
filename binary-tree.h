#ifndef BINARY_TREE_H
#define BINARY_TREE_H

// very simple array based binary tree implementation, for plru

int* btree_init(int number_of_nodes); // fixed size because it won't grow and shrink
                                      // we are simulating hardware stuff here
void btree_cleanup(int* tree);

int btree_get_left_child(int index);
int btree_get_right_child(int index);
int btree_get_parent(int index);

#endif