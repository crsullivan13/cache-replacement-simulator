#include <stdio.h>
#include <string.h>

#include "util.h"

bool isValidConfig(const char* config) {
    if ( config == NULL ) {
        fprintf(stderr, "ERROR: Config was NULL\n");
        return false;
    } else {
        return  ( strncmp("random", config, strlen("random")) == 0 ||
            strncmp("lru", config, strlen("lru")) == 0 ||
            strncmp("plru", config, strlen("plru")) == 0 );
    }
}