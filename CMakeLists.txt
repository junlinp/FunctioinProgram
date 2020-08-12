cmake_minimum_required(VERSION 3.13)
project(FP)
set(CMAKE_CXX_STANDARD 14)
find_package(GTest REQUIRED)

add_executable(function_program_test function_program_test.cpp)
target_link_libraries(function_program_test PUBLIC GTest::GTest)
add_test(NAME function_pgram_test COMMAND function_program_test)