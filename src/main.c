#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "cache.h"
#include "logging.h"
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

    srand(time(NULL));

    int address = 0;
    int set = 0;
    int tag = 0;
    int set_bits = findPowerOf2(my_cache.number_of_cache_sets);
    int set_mask = ( ( 1 << set_bits ) - 1 );
    LOG("Set mask is %x\n", set_mask << CACHE_LINE_LOG2);

    int number_of_addresses = 16384;
    uint64_t* addresses = gen_rand_addresses(number_of_addresses);

    for ( int i = 0; i < number_of_addresses; i++ ) {
        address = addresses[i];
        set = ( address >> CACHE_LINE_LOG2 ) & ( set_mask );
        tag = ( address >> ( CACHE_LINE_LOG2 + set_bits ) );
        LOG("Address is %x, set is %d, tag is %d\n", address, set, tag);

        if (  !is_cache_hit(&my_cache, set ,tag) ) {
            LOG("Cache MISS\n");
            cache_directory_write(&my_cache, set, cache_select_victim_way(&my_cache, set, my_cache.associativity), tag);
        } else {
            LOG("Cache HIT\n");
        }

        LOG("Is access a hit after line fill? %d\n", is_cache_hit(&my_cache, set, tag));
    }

    free(addresses);
    cache_cleanup(&my_cache);
}