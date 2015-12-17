cmake_minimum_required(VERSION 2.8)
PROJECT(Evolution)

add_subdirectory(C-Utils)
add_library(evolution STATIC evolution)
target_link_libraries(evolution Thread-Client m pthread)
