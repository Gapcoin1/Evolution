cmake_minimum_required(VERSION 2.8)
PROJECT(Evolution)

add_subdirectory("./C-Utils")
add_library(Evolution STATIC evolution.c C-Utils)
