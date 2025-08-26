#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

static bool isValidConfig(const char* config) {
    if ( config == NULL ) {
        fprintf(stderr, "ERROR: Config was NULL\n");
        return false;
    } else {
        return  ( strncmp("random", config, strlen("random")) == 0 ||
            strncmp("lru", config, strlen("lru")) == 0 ||
            strncmp("plru", config, strlen("plru")) == 0 );
    }
}

// non-powers of 2 will waste address bits
static int findPowerOf2(int value) {
    int power = 0;
    
    while ( ( 1 << power ) < value ) {
        power += 1;
    }

    return power;
}

// must call free on returned list
static uint64_t* gen_rand_addresses(int number_of_addresses) {
    uint64_t* addresses = (uint64_t*) malloc(sizeof(uint64_t) * number_of_addresses);

    for ( int i = 0; i < number_of_addresses; i++) {
        addresses[i] = rand();
    }

    return addresses;
}

#endif