#pragma once

#include "command_base.hpp"

namespace poker {

class PostflopAnalysisCommand : public CommandBase {
public:
    PostflopAnalysisCommand();
    ~PostflopAnalysisCommand() override = default;
    
    void execute() override;
    std::string get_name() const override;
    std::string get_description() const override;
    
private:
    void print_help() const;
    void show_progress(int completed, int total, const std::string& current_item);
    
    // Generate representative board textures for analysis
    std::vector<std::string> generate_representative_boards() const;
    
    // Board texture categorization
    std::vector<std::string> get_dry_boards() const;
    std::vector<std::string> get_wet_boards() const;
    std::vector<std::string> get_paired_boards() const;
    std::vector<std::string> get_high_boards() const;
    std::vector<std::string> get_middle_boards() const;
    std::vector<std::string> get_low_boards() const;
    
    // Generate action sequences for analysis
    std::vector<std::string> generate_action_sequences() const;
    
    // Create comprehensive output structure
    void create_output_directories(const std::string& base_path) const;
    
    // Analysis phase implementations
    void generate_flop_analysis(PokerAnalyzer& analyzer, const Config& config);
    void generate_turn_analysis(PokerAnalyzer& analyzer, const Config& config);
    void generate_river_analysis(PokerAnalyzer& analyzer, const Config& config);
};

} // namespace poker
