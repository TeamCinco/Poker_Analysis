#pragma once

#include "command_base.hpp"

namespace poker {

class RealisticRangesCommand : public CommandBase {
public:
    RealisticRangesCommand();
    ~RealisticRangesCommand() override = default;
    
    void execute() override;
    std::string get_name() const override;
    std::string get_description() const override;
    
private:
    void print_help() const;
    void show_progress(int completed, int total, const std::string& current_item);
};

} // namespace poker
