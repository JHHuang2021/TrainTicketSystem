cmake_minimum_required(VERSION 3.16)
project(Bookstore)

set(CMAKE_CXX_STANDARD 20)
add_compile_options(-O2)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
# set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -fsanitize=address")

include_directories(
  ${PROJECT_SOURCE_DIR}/BPlusTree/
)

set(src_dir
  # ${PROJECT_SOURCE_DIR}/BPlusTree/disk_manager.cpp
  # ${PROJECT_SOURCE_DIR}/BPlusTree/buffer_pool_manager.cpp

  # ${PROJECT_SOURCE_DIR}/BPlusTree/test.cpp
  # src/b_plus_tree/disk_manager.cpp
  # src/b_plus_tree/buffer_pool_manager.cpp
  src/main.cpp
  src/command_parser.cpp
  src/user.cpp
  src/train.cpp
  src/lib/datetime.cpp
)

add_executable(code ${src_dir})