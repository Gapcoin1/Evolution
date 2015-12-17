cmake_minimum_required(VERSION 2.8)
PROJECT(Evolution)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(ARC ${CMAKE_BINARY_DIR}/lib)
set(LIB ${CMAKE_BINARY_DIR}/lib)
set(RUN ${CMAKE_BINARY_DIR}/bin)

add_subdirectory("./src")

add_executable(last_test test/last_test)
add_executable(test_only_mutate test/test-only-mutate)
add_executable(test_parallel test/test-parallel)
add_executable(tsp test/tsp)

target_link_libraries(last_test evolution)
target_link_libraries(test_only_mutate evolution)
target_link_libraries(test_parallel evolution)
target_link_libraries(tsp evolution)


enable_testing()

add_test(last_test_seriel ${RUN}/last_test 100 1 0 100 10)
add_test(last_test_parallel ${RUN}/last_test 100 4 0 100 10)
add_test(only_mutate ${RUN}/test_only_mutate 100)
add_test(parallel ${RUN}/test_parallel 100 4 0)
add_test(tsp_test ${RUN}/tsp 100 1000 100 4 0 0)
add_test(tsp_test_greedy ${RUN}/tsp 100 1000 100 4 0 1)
