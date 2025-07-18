#pragma once

#include "types.hpp"
#include "monte_carlo_engine.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>

namespace poker {

class PokerAnalyzer {
public:
    PokerAnalyzer(const Config& config = Config{});
    ~PokerAnalyzer();
    
    // Main analysis functions
    HandAnalysis analyze_hand(const std::string& hand_str, int simulations_per_opponent = 1000000);
    
    // Batch analysis for all 169 starting hands
    std::unordered_map<std::string, HandAnalysis> analyze_all_hands(int simulations_per_hand = 1000000);
    
    // Position-specific analysis
    HandAnalysis analyze_hand_position(const std::string& hand_str, Position position, int simulations = 1000000);
    
    // Range analysis
    std::vector<HandAnalysis> analyze_range(const std::vector<std::string>& range, int simulations_per_hand = 1000000);
    
    // Scenario-specific analysis
    HandAnalysis analyze_3bet_scenario(const std::string& hand_str, Position hero_pos, Position opener_pos, int simulations = 1000000);
    HandAnalysis analyze_4bet_scenario(const std::string& hand_str, Position hero_pos, Position three_bettor_pos, int simulations = 1000000);
    
    // Stack depth analysis
    HandAnalysis analyze_stack_depth(const std::string& hand_str, int stack_bb, int simulations = 1000000);
    
    // Quick analysis for real-time use
    HandAnalysis quick_analyze(const std::string& hand_str, int max_time_ms = 100);
    
    // Performance and configuration
    PerformanceStats get_performance_stats() const;
    void set_config(const Config& config);
    
    // Generate comprehensive data for Python trainer
    void generate_training_data(const std::string& output_directory);
    
    // Progress callback for long-running operations
    using ProgressCallback = std::function<void(int completed, int total, const std::string& current_hand)>;
    void set_progress_callback(ProgressCallback callback);

private:
    std::unique_ptr<MonteCarloEngine> engine_;
    Config config_;
    ProgressCallback progress_callback_;
    
    // Generate all 169 starting hands
    std::vector<std::string> generate_all_starting_hands() const;
    
    // Determine hand combinations
    int get_hand_combinations(const std::string& hand_str) const;
    
    // Calculate equity vs different opponent ranges
    double calculate_equity_vs_range(const std::string& hand_str, const std::vector<std::string>& range, int simulations) const;
    
    // GTO analysis
    void analyze_gto_frequencies(HandAnalysis& analysis, const std::string& hand_str) const;
    
    // Position-specific calculations
    void calculate_position_actions(HandAnalysis& analysis, const std::string& hand_str) const;
    
    // Expected value calculations
    double calculate_expected_value(const std::string& hand_str, Position position, Action action) const;
    
    // Preflop ranges for different positions
    std::vector<std::string> get_position_range(Position position) const;
    std::vector<std::string> get_tight_range() const;
    std::vector<std::string> get_loose_range() const;
    
    // Statistical validation
    bool validate_hand_analysis(const HandAnalysis& analysis) const;
    
    // Data export functions
    void export_hand_equity_data(const std::string& output_path);
    void export_position_ranges(const std::string& output_path) const;
    void export_scenario_analysis(const std::string& output_path) const;
    void export_stack_depth_adjustments(const std::string& output_path) const;
    
    // Utility functions
    std::string format_hand_analysis(const HandAnalysis& analysis) const;
    void log_progress(const std::string& message) const;
};

} // namespace poker
