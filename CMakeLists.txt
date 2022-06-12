cmake_minimum_required(VERSION 3.16)
project(Bookstore)

set(CMAKE_CXX_STANDARD 20)
#add_compile_options(-std=c++2a -fconcepts)
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -fsanitize=address")

include_directories(
  ${PROJECT_SOURCE_DIR}/BPlusTree/
)

set(src_dir
  # ${PROJECT_SOURCE_DIR}/BPlusTree/disk_manager.cpp
  # ${PROJECT_SOURCE_DIR}/BPlusTree/buffer_pool_manager.cpp

  # ${PROJECT_SOURCE_DIR}/BPlusTree/test.cpp
  src/main.cpp
  src/command_parser.cpp
  src/user.cpp
  src/train.cpp
  src/lib/datetime.cpp
)

add_executable(code ${src_dir})