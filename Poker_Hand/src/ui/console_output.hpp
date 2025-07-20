#pragma once

#include <string>

namespace poker {
namespace ui {

/**
 * @brief Console output utilities for the poker analyzer
 * 
 * This class provides static methods for printing formatted output
 * to the console, including banners, system information, and usage instructions.
 */
class ConsoleOutput {
public:
    /**
     * @brief Print the application banner
     */
    static void print_banner();
    
    /**
     * @brief Print system information including CPU, compiler, and SIMD support
     */
    static void print_system_info();
    
    /**
     * @brief Print usage instructions for command-line arguments
     */
    static void print_usage();
    
    /**
     * @brief Print completion message
     */
    static void print_completion();
    
    /**
     * @brief Print error message for unknown command
     */
    static void print_unknown_command(const std::string& command);
};

} // namespace ui
} // namespace poker
