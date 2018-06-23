cmake_minimum_required(VERSION 3.8)
project(my_deque)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address,undefined -g -D_GLIBCXX_DEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_executable(my_deque main.cpp deque.h)