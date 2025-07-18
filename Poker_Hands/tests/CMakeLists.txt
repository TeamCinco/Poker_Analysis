# Test framework for Poker Analytics
cmake_minimum_required(VERSION 3.10)

# Test executable
add_executable(poker_tests test_main.cpp)

# Link with the poker library
target_link_libraries(poker_tests poker_lib)

# Include directories
target_include_directories(poker_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Add test
add_test(NAME poker_basic_tests COMMAND poker_tests)
