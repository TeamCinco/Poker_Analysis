cmake_minimum_required(VERSION 3.10)
project(PokerAnalytics VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Platform detection
if(WIN32)
    set(PLATFORM_WINDOWS TRUE)
    message(STATUS "Building for Windows")
elseif(APPLE)
    set(PLATFORM_MAC TRUE)
    message(STATUS "Building for macOS")
elseif(UNIX)
    set(PLATFORM_LINUX TRUE)
    message(STATUS "Building for Linux")
endif()

# Include directories
include_directories(include)

# Source files
set(SOURCES
    src/card.cpp
    src/hand.cpp
    src/analyzer.cpp
    src/simulator.cpp
    src/platform_utils.cpp
)

# Create library
add_library(poker_lib ${SOURCES})

# Main executable
add_executable(poker_analytics main.cpp)
target_link_libraries(poker_analytics poker_lib)

# Platform-specific settings
if(PLATFORM_WINDOWS)
    # Windows-specific settings
    target_compile_definitions(poker_lib PRIVATE _WIN32_WINNT=0x0601)
    target_compile_options(poker_lib PRIVATE /utf-8)
    if(MSVC)
        target_compile_options(poker_lib PRIVATE /W4 /permissive-)
        target_compile_options(poker_analytics PRIVATE /W4 /permissive-)
    endif()
elseif(PLATFORM_MAC OR PLATFORM_LINUX)
    # Unix-like systems
    target_compile_options(poker_lib PRIVATE -Wall -Wextra -pedantic)
    target_compile_options(poker_analytics PRIVATE -Wall -Wextra -pedantic)
    
    # Link required libraries for Unix systems
    find_package(Threads REQUIRED)
    target_link_libraries(poker_lib Threads::Threads)
endif()

# Compiler-specific optimizations
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(MSVC)
        target_compile_options(poker_lib PRIVATE /O2)
        target_compile_options(poker_analytics PRIVATE /O2)
    else()
        target_compile_options(poker_lib PRIVATE -O3 -DNDEBUG)
        target_compile_options(poker_analytics PRIVATE -O3 -DNDEBUG)
    endif()
endif()

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Optional: Add testing support
option(BUILD_TESTS "Build test programs" ON)
if(BUILD_TESTS)
    enable_testing()
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/CMakeLists.txt")
        add_subdirectory(tests)
    endif()
endif()

# Installation rules
install(TARGETS poker_analytics DESTINATION bin)
install(TARGETS poker_lib DESTINATION lib)
install(DIRECTORY include/ DESTINATION include)

# Print build configuration
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
