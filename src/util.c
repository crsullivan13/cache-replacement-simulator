#include <string.h>

#include "util.h"

bool isValidConfig(const char* config) {
    return  ( strncmp("random", config, strlen("random")) == 0 ||
               strncmp("lru", config, strlen("lru")) == 0 ||
               strncmp("plru", config, strlen("plru")) == 0 );
}