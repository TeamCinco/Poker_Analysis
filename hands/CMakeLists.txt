cmake_minimum_required(VERSION 3.16)
project(MonteCarloPoker)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Create output directory
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/output)

# Include directories
include_directories(src)

# Source files
file(GLOB_RECURSE SOURCES 
    "src/*.cpp"
    "src/*.hpp"
)

# Create executable
add_executable(monte_carlo_poker ${SOURCES})

# Copy output directory to build location
add_custom_command(TARGET monte_carlo_poker POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:monte_carlo_poker>/output
)

# Compiler-specific flags
if(MSVC)
    target_compile_options(monte_carlo_poker PRIVATE /W4 /O2)
else()
    target_compile_options(monte_carlo_poker PRIVATE -Wall -Wextra -O2)
endif()
