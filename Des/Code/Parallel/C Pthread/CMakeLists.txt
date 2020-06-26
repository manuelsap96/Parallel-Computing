cmake_minimum_required(VERSION 3.16)
project(DesDecription C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")

add_executable(DesDecription  des_parallel.c)
target_link_libraries(DesDecription crypt)
target_link_libraries(DesDecription pthread)