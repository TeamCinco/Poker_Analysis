#pragma once

#include "../core/types.hpp"
#include "../engine/monte_carlo_engine.hpp"
#include <vector>
#include <string>

namespace poker {

class PokerAnalyzer {
public:
    PokerAnalyzer();
    
    // Main analysis functions
    void analyze_all_table_sizes();
    void analyze_table_size(int table_size);
    
    // Export results to CSV
    void export_results_to_csv(int table_size);
    
    // Get progress information
    int get_total_scenarios() const { return total_scenarios_; }
    int get_completed_scenarios() const { return completed_scenarios_; }

private:
    MonteCarloEngine engine_;
    
    // Results storage
    std::vector<std::pair<Scenario, SimResult>> results_;
    
    // Progress tracking
    int total_scenarios_;
    int completed_scenarios_;
    
    // Hand lists
    static const std::vector<std::string> ALL_HANDS;
    static const std::vector<Position> ALL_POSITIONS;
    static const std::vector<std::string> PREFLOP_ACTIONS;
    static const std::vector<std::string> POSTFLOP_ACTIONS;
    static const std::vector<std::string> SAMPLE_BOARDS;
    
    // Analysis functions
    void analyze_preflop_scenarios(int table_size);
    void analyze_postflop_scenarios(int table_size);
    
    // Scenario generation
    std::vector<Scenario> generate_preflop_scenarios(int table_size);
    std::vector<Scenario> generate_flop_scenarios(int table_size);
    std::vector<Scenario> generate_turn_scenarios(int table_size);
    std::vector<Scenario> generate_river_scenarios(int table_size);
    
    // Helper functions
    void clear_results();
    void print_progress(const std::string& message);
    std::string get_output_directory(int table_size);
    void create_output_directories();
};

} // namespace poker
