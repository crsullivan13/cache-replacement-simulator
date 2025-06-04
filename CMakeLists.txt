cmake_minimum_required(VERSION 3.5.0)
project(CacheReplacement VERSION 0.1.0 LANGUAGES C CXX)

add_executable(CacheReplacement main.c cache.h cache.c util.h util.c binary-tree.h binary-tree.c lru_list.h lru_list.c)
add_compile_options(-Wall)

include(CTest)
enable_testing()

