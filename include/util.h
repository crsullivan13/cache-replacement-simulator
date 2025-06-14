#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>
#include <stdint.h>

bool isValidConfig(const char* config);
int findPowerOf2(int value);
uint64_t* gen_rand_addresses(int number_of_addresses);

#endif