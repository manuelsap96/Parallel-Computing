cmake_minimum_required(VERSION 3.15)
project(n_grams_par_c__)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fopenmp ")

add_executable(n_grams_par_c__ nGramsGenerator.cpp)

