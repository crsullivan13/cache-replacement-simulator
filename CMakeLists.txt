cmake_minimum_required(VERSION 3.5.0)
project(CacheReplacement VERSION 0.1.0 LANGUAGES C CXX)

set(SOURCES src/main.cpp src/cache.cpp 
            src/binary-tree.cpp src/lru_list.cpp src/logging.cpp)

add_compile_options(-Wall -DLOGGING) # Probably a better way, but define logging this way for now
add_executable(CacheReplacement ${SOURCES})
target_include_directories(CacheReplacement PRIVATE "${PROJECT_SOURCE_DIR}/include")

include(CTest)
enable_testing()

