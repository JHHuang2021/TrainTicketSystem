cmake_minimum_required(VERSION 3.16)
project(Bookstore)

set(CMAKE_CXX_STANDARD 20)
#add_compile_options(-std=c++2a -fconcepts)
add_compile_options(-g)

include_directories(
        ${PROJECT_SOURCE_DIR}/src/
)

set(src_dir
        ${PROJECT_SOURCE_DIR}/src/b_plus_tree/disk_manager.cpp
        ${PROJECT_SOURCE_DIR}/src/b_plus_tree/buffer_pool_manager.cpp

        ${PROJECT_SOURCE_DIR}/src/b_plus_tree/test.cpp
        )

add_executable(code ${src_dir})