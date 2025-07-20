#pragma once

#include "command_base.hpp"
#include <string>
#include <memory>
#include <unordered_map>

namespace poker {

/**
 * @brief Registry for managing and executing poker analysis commands
 * 
 * This class provides a centralized way to register, manage, and execute
 * different poker analysis commands. It supports easy extensibility by
 * allowing new commands to be registered at runtime.
 */
class CommandRegistry {
public:
    /**
     * @brief Register all available commands
     * 
     * This method registers all built-in commands with the registry.
     * New commands should be added here when implementing new simulations.
     */
    static void register_all_commands();
    
    /**
     * @brief Get a command by name
     * 
     * @param command_name The name of the command to retrieve
     * @return Pointer to the command, or nullptr if not found
     */
    static CommandBase* get_command(const std::string& command_name);
    
    /**
     * @brief Check if a command exists
     * 
     * @param command_name The name of the command to check
     * @return True if the command exists, false otherwise
     */
    static bool has_command(const std::string& command_name);
    
    /**
     * @brief Get the default command name
     * 
     * @return The name of the default command to run when none is specified
     */
    static std::string get_default_command();

private:
    /**
     * @brief Register a single command
     * 
     * @param command Unique pointer to the command to register
     */
    static void register_command(std::unique_ptr<CommandBase> command);
    
    /// Static registry of all available commands
    static std::unordered_map<std::string, std::unique_ptr<CommandBase>> commands_;
};

} // namespace poker
