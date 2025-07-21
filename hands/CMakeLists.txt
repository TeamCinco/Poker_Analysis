cmake_minimum_required(VERSION 3.16)
project(MonteCarloPoker)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable aggressive optimization for release builds
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Create output directory
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/output)

# Include directories
include_directories(src)

# Find required packages
find_package(Threads REQUIRED)

# Source files - explicitly list for better control
set(CORE_SOURCES
    src/core/types.cpp
    src/core/deck.cpp
    src/core/hand_evaluator.cpp
)

set(ENGINE_SOURCES
    src/engine/monte_carlo_engine.cpp
    src/engine/thread_pool.cpp
    src/engine/simd_optimizer.cpp
)

set(ANALYSIS_SOURCES
    src/analysis/poker_analyzer.cpp
    src/analysis/decision_engine.cpp
    src/analysis/accelerated_decision_engine.cpp
    src/analysis/dynamic_monte_carlo_analyzer.cpp
)

set(MAIN_SOURCES
    src/main.cpp
)

set(DYNAMIC_MAIN_SOURCES
    src/dynamic_main.cpp
)

# Combine all sources for original executable
set(ALL_SOURCES 
    ${CORE_SOURCES}
    ${ENGINE_SOURCES}
    ${ANALYSIS_SOURCES}
    ${MAIN_SOURCES}
)

# Combine all sources for dynamic executable
set(DYNAMIC_ALL_SOURCES
    ${CORE_SOURCES}
    ${ENGINE_SOURCES}
    ${ANALYSIS_SOURCES}
    ${DYNAMIC_MAIN_SOURCES}
)

# Create original executable
add_executable(monte_carlo_poker ${ALL_SOURCES})

# Create new dynamic boundary-driven executable
add_executable(dynamic_poker_analyzer ${DYNAMIC_ALL_SOURCES})

# Link threading library to both executables
target_link_libraries(monte_carlo_poker PRIVATE Threads::Threads)
target_link_libraries(dynamic_poker_analyzer PRIVATE Threads::Threads)

# Copy output directory to build location for both executables
add_custom_command(TARGET monte_carlo_poker POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:monte_carlo_poker>/output
)
add_custom_command(TARGET dynamic_poker_analyzer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:dynamic_poker_analyzer>/output
)

# High-performance compiler flags for both executables
if(MSVC)
    target_compile_options(monte_carlo_poker PRIVATE 
        /W4                    # High warning level
        /O2                    # Optimize for speed
        /Ot                    # Favor fast code
        /GL                    # Whole program optimization
        /arch:AVX2             # Enable AVX2 instructions
        /fp:fast               # Fast floating point
        /openmp                # OpenMP support
    )
    target_link_options(monte_carlo_poker PRIVATE 
        /LTCG                  # Link-time code generation
    )
    target_compile_options(dynamic_poker_analyzer PRIVATE 
        /W4                    # High warning level
        /O2                    # Optimize for speed
        /Ot                    # Favor fast code
        /GL                    # Whole program optimization
        /arch:AVX2             # Enable AVX2 instructions
        /fp:fast               # Fast floating point
        /openmp                # OpenMP support
    )
    target_link_options(dynamic_poker_analyzer PRIVATE 
        /LTCG                  # Link-time code generation
    )
else()
    # GCC/Clang flags for monte_carlo_poker
    target_compile_options(monte_carlo_poker PRIVATE 
        -Wall -Wextra         # Warnings
        -O3                   # Maximum optimization
        -march=native         # Use all available CPU features
        -mtune=native         # Tune for current CPU
        -mavx2                # Enable AVX2
        -mfma                 # Enable FMA instructions
        -funroll-loops        # Unroll loops
        -ffast-math           # Fast math optimizations
        -flto                 # Link-time optimization
        -fopenmp              # OpenMP support
    )
    target_link_options(monte_carlo_poker PRIVATE 
        -flto                 # Link-time optimization
        -fopenmp              # OpenMP support
    )
    # GCC/Clang flags for dynamic_poker_analyzer
    target_compile_options(dynamic_poker_analyzer PRIVATE 
        -Wall -Wextra         # Warnings
        -O3                   # Maximum optimization
        -march=native         # Use all available CPU features
        -mtune=native         # Tune for current CPU
        -mavx2                # Enable AVX2
        -mfma                 # Enable FMA instructions
        -funroll-loops        # Unroll loops
        -ffast-math           # Fast math optimizations
        -flto                 # Link-time optimization
        -fopenmp              # OpenMP support
    )
    target_link_options(dynamic_poker_analyzer PRIVATE 
        -flto                 # Link-time optimization
        -fopenmp              # OpenMP support
    )
endif()

# Enable additional optimizations for Release builds
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(MSVC)
        target_compile_definitions(monte_carlo_poker PRIVATE NDEBUG)
        target_compile_definitions(dynamic_poker_analyzer PRIVATE NDEBUG)
    else()
        target_compile_options(monte_carlo_poker PRIVATE -DNDEBUG)
        target_compile_options(dynamic_poker_analyzer PRIVATE -DNDEBUG)
    endif()
endif()

# Print build configuration
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
if(MSVC)
    message(STATUS "MSVC version: ${CMAKE_CXX_COMPILER_VERSION}")
else()
    message(STATUS "Compiler version: ${CMAKE_CXX_COMPILER_VERSION}")
endif()
message(STATUS "Optimizations: Enabled (AVX2, Multi-threading, LTO)")
