#include "commands/command_registry.hpp"
#include "ui/console_output.hpp"
#include <iostream>

using namespace poker;

int main(int argc, char* argv[]) {
    // Display application banner and system information
    ui::ConsoleOutput::print_banner();
    ui::ConsoleOutput::print_system_info();
    
    // Register all available commands
    CommandRegistry::register_all_commands();
    
    // Determine which command to run
    std::string command_name = CommandRegistry::get_default_command();
    if (argc > 1) {
        command_name = argv[1];
    }
    
    // Handle help command
    if (command_name == "help" || command_name == "--help" || command_name == "-h") {
        ui::ConsoleOutput::print_usage();
        return 0;
    }
    
    // Execute the requested command
    CommandBase* command = CommandRegistry::get_command(command_name);
    if (command) {
        try {
            command->execute();
            ui::ConsoleOutput::print_completion();
            return 0;
        } catch (const std::exception& e) {
            std::cerr << "Error executing command '" << command_name << "': " << e.what() << std::endl;
            return 1;
        }
    } else {
        ui::ConsoleOutput::print_unknown_command(command_name);
        ui::ConsoleOutput::print_usage();
        return 1;
    }
}
