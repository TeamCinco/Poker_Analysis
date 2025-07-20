cmake_minimum_required(VERSION 3.16)
project(PokerHandAnalysis VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler-specific optimizations
if(MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /Oi /Ot /arch:AVX2 /fp:fast /GL")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG")
else()
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native -mtune=native -ffast-math -flto")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -flto")
endif()

# Find required packages
find_package(Threads REQUIRED)

# Include directories
include_directories(src)
include_directories(src/core)
include_directories(src/engine)
include_directories(src/analysis)
include_directories(src/utils)
include_directories(src/commands)
include_directories(src/ui)

# Source files
set(SOURCES
    src/main.cpp
    src/core/card.cpp
    src/core/hand_evaluator.cpp
    src/core/deck.cpp
    src/engine/monte_carlo_engine.cpp
    src/engine/parallel_simulator.cpp
    src/engine/simd_optimizer.cpp
    src/analysis/poker_analyzer.cpp
    src/analysis/hand_ranges.cpp
    src/analysis/realistic_ranges.cpp
    src/analysis/equity_calculator.cpp
    src/utils/json_output.cpp
    src/utils/csv_exporter.cpp
    src/utils/performance_profiler.cpp
    src/utils/cpu_features.cpp
    src/commands/command_registry.cpp
    src/commands/training_data_command.cpp
    src/commands/realistic_ranges_command.cpp
    src/ui/console_output.cpp
    src/ui/progress_display.cpp
)

# Create executable
add_executable(poker_analysis_generator ${SOURCES})

# Link libraries
target_link_libraries(poker_analysis_generator Threads::Threads)

# Compiler-specific flags
if(MSVC)
    target_compile_options(poker_analysis_generator PRIVATE /W4)
else()
    target_compile_options(poker_analysis_generator PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Create output directory
add_custom_target(create_output_dir ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/output
)

# Copy to output directory after build
add_custom_command(TARGET poker_analysis_generator POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:poker_analysis_generator> ${CMAKE_SOURCE_DIR}/output/
)
