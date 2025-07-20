#include "command_registry.hpp"
#include "training_data_command.hpp"

namespace poker {

std::unordered_map<std::string, std::unique_ptr<CommandBase>> CommandRegistry::commands_;

void CommandRegistry::register_all_commands() {
    // Register training data command
    register_command(std::make_unique<TrainingDataCommand>());
}

CommandBase* CommandRegistry::get_command(const std::string& command_name) {
    auto it = commands_.find(command_name);
    return (it != commands_.end()) ? it->second.get() : nullptr;
}

bool CommandRegistry::has_command(const std::string& command_name) {
    return commands_.find(command_name) != commands_.end();
}

std::string CommandRegistry::get_default_command() {
    return "data";
}

void CommandRegistry::register_command(std::unique_ptr<CommandBase> command) {
    if (command) {
        std::string name = command->get_name();
        commands_[name] = std::move(command);
    }
}

} // namespace poker
