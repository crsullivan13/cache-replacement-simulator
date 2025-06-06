#include <stdio.h>
#include <stdlib.h>

#include "cache.h"
#include "util.h"

int main(int argc, char* argv[]) {

    if ( argc != 2 ) {
        printf("USAGE: ./CacheReplacement <random|plru|lru>\n");
        exit(0);
    }

    if ( !isValidConfig(argv[1]) ) {
        printf("Invalid config! Valid configs are <random|plru|lru>\n");
        exit(0);
    }

    printf("Hello cache: policy is %s\n", argv[1]);

    cache_t my_cache;
    if ( cache_create(&my_cache, cache_convert_policy(argv[1]), 1024, 16) != 0 ) {
        fprintf(stderr, "ERROR: Cache creation failed\n");
        exit(1);
    }

    for ( int i = 0; i < my_cache.associativity * 2; i++ ) {
        int victim_way = cache_select_victim_way(&my_cache, 5, my_cache.associativity);
        cache_directory_write(&my_cache, 5, victim_way);
    }

    cache_cleanup(&my_cache);
}