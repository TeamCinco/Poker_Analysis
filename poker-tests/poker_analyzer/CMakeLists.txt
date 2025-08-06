cmake_minimum_required(VERSION 3.10)
project(PokerFlopAnalyzer VERSION 1.0.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set build type to Release by default for better performance
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler-specific options
if(MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "/O2 /DNDEBUG")
    set(CMAKE_CXX_FLAGS_DEBUG "/Od /DEBUG")
else()
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
endif()

# Include directories
include_directories(src)

# Source files
set(SOURCES
    src/main.cpp
    src/poker_analyzer.cpp
)

set(HEADERS
    src/poker_analyzer.hpp
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Enable threading support
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} Threads::Threads)

# Platform-specific settings
if(WIN32)
    # Windows specific settings
    target_compile_definitions(${PROJECT_NAME} PRIVATE WIN32_LEAN_AND_MEAN)
endif()

# Set output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Copy any resource files if needed (uncomment if you have config files)
# configure_file(${CMAKE_SOURCE_DIR}/config.ini ${CMAKE_BINARY_DIR}/bin/config.ini COPYONLY)

# Print build information
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")