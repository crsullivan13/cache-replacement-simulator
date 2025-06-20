cmake_minimum_required(VERSION 3.5.0)
project(CacheReplacement VERSION 0.1.0 LANGUAGES C CXX)

set(SOURCES src/main.c src/cache.c 
            src/binary-tree.c src/lru_list.c src/logging.c)

add_compile_options(-Wall ) # Probably a better way, but define logging this way for now
add_executable(CacheReplacement ${SOURCES})
target_include_directories(CacheReplacement PRIVATE "${PROJECT_SOURCE_DIR}/include")

include(CTest)
enable_testing()

