cmake_minimum_required(VERSION 3.5.0)
project(CacheReplacement VERSION 0.1.0 LANGUAGES C CXX)

set(SOURCES src/main.c src/cache.c src/util.c 
            src/binary-tree.c src/lru_list.c)

add_compile_options(-Wall)
add_executable(CacheReplacement ${SOURCES})
target_include_directories(CacheReplacement PRIVATE "${PROJECT_SOURCE_DIR}/include")

include(CTest)
enable_testing()

