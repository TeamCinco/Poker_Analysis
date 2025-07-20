#include "console_output.hpp"
#include <iostream>
#include <thread>

namespace poker {
namespace ui {

void ConsoleOutput::print_banner() {
    std::cout << "==========================================" << std::endl;
    std::cout << "  High-Performance Poker Hand Analyzer   " << std::endl;
    std::cout << "       C++ Monte Carlo Engine            " << std::endl;
    std::cout << "==========================================" << std::endl;
    std::cout << std::endl;
}

void ConsoleOutput::print_system_info() {
    std::cout << "System Information:" << std::endl;
    std::cout << "  CPU Cores: " << std::thread::hardware_concurrency() << std::endl;
    std::cout << "  Compiler: ";
#ifdef _MSC_VER
    std::cout << "MSVC " << _MSC_VER << std::endl;
#elif defined(__GNUC__)
    std::cout << "GCC " << __GNUC__ << "." << __GNUC_MINOR__ << std::endl;
#elif defined(__clang__)
    std::cout << "Clang " << __clang_major__ << "." << __clang_minor__ << std::endl;
#else
    std::cout << "Unknown" << std::endl;
#endif
    std::cout << "  SIMD Support: ";
#ifdef __AVX2__
    std::cout << "AVX2 ";
#endif
#ifdef __AVX512F__
    std::cout << "AVX-512 ";
#endif
    std::cout << std::endl;
    std::cout << "  Target Performance: 10M+ simulations/second" << std::endl;
    std::cout << "  Expected Speedup: 50-200x vs Python" << std::endl;
    std::cout << std::endl;
}

void ConsoleOutput::print_usage() {
    std::cout << "Usage: poker_analysis_generator [mode]" << std::endl;
    std::cout << "  data    - Generate comprehensive poker training data (default)" << std::endl;
    std::cout << std::endl;
}

void ConsoleOutput::print_completion() {
    std::cout << "Program completed successfully!" << std::endl;
}

void ConsoleOutput::print_unknown_command(const std::string& command) {
    std::cout << "Unknown mode: " << command << std::endl;
}

} // namespace ui
} // namespace poker
