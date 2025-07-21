#pragma once

#include "../core/types.hpp"
#include "../engine/monte_carlo_engine.hpp"
#include "../engine/thread_pool.hpp"
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <map>
#include <atomic>
#include <random>

namespace poker {

// Dynamic boundary parameters - the ONLY hardcoded parts
struct AnalysisLimits {
    // Hand range limits
    std::vector<std::string> rank_range = {"A", "K", "Q", "J", "T", "9", "8", "7", "6", "5", "4", "3", "2"};
    std::vector<std::string> suit_combinations = {"suited", "offsuit", "pair"};
    
    // Position limits  
    std::vector<Position> positions = {Position::UTG, Position::HJ, Position::CO, Position::BTN, Position::SB, Position::BB};
    
    // Street limits
    std::vector<Street> streets = {Street::PREFLOP, Street::FLOP, Street::TURN, Street::RIVER};
    
    // Action limits
    std::vector<std::string> actions = {"open", "3bet", "4bet", "5bet", "call", "fold", "cbet", "check", "raise", "bet"};
    
    // Board texture limits
    int max_board_samples = 50;  // How many random boards to test per street
    
    // Frequency testing limits
    std::vector<double> test_frequencies = {0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
    
    // Simulation limits
    int simulations_per_test = 50000;
    int max_action_depth = 6;  // Max actions in a sequence
    double min_profitable_ev = 0.01;  // Minimum EV to consider profitable
    
    // Performance limits
    int max_threads = 0;  // 0 = use all available cores
    bool use_simd_optimization = true;
    
    // Table size limits
    std::vector<int> table_sizes = {6, 7, 8, 9};
};

// Discovered strategy entry - only profitable strategies are stored
struct DiscoveredStrategy {
    std::string hand;                    // "AA", "KQs", etc.
    Position position;                   // UTG, BTN, etc.
    std::vector<std::string> actions;    // Action sequence leading to this decision
    std::string board;                   // Board texture (if postflop)
    Street street;                       // Current street
    std::string decision_type;           // The profitable action discovered
    int table_size;                      // 6, 7, 8, or 9
    
    double optimal_frequency;            // Optimal frequency (0.0 to 1.0)
    double expected_value;              // EV at optimal frequency
    int simulations_run;                // Number of simulations used
    double confidence_interval;         // Statistical confidence
    
    std::string get_unique_id() const;
};

// Dynamic hand combination generator
class DynamicHandGenerator {
public:
    DynamicHandGenerator(const AnalysisLimits& limits);
    
    // Generate all possible hand combinations within limits
    std::vector<std::string> generate_all_hands() const;
    
    // Generate specific subset based on criteria
    std::vector<std::string> generate_premium_hands() const;
    std::vector<std::string> generate_speculative_hands() const;
    std::vector<std::string> generate_hands_by_strength(double min_strength, double max_strength) const;

private:
    const AnalysisLimits& limits_;
    
    // Helper functions
    std::vector<std::string> generate_pairs() const;
    std::vector<std::string> generate_suited_hands() const;
    std::vector<std::string> generate_offsuit_hands() const;
    double calculate_hand_strength(const std::string& hand) const;
};

// Dynamic board generator
class DynamicBoardGenerator {
public:
    DynamicBoardGenerator(const AnalysisLimits& limits);
    
    // Generate representative boards for each street
    std::vector<std::string> generate_flop_boards() const;
    std::vector<std::string> generate_turn_boards() const;
    std::vector<std::string> generate_river_boards() const;
    
    // Generate boards by texture type
    std::vector<std::string> generate_dry_boards() const;
    std::vector<std::string> generate_wet_boards() const;
    std::vector<std::string> generate_paired_boards() const;

private:
    const AnalysisLimits& limits_;
    mutable std::mt19937 rng_;
    
    // Board classification
    bool is_dry_board(const std::string& board) const;
    bool is_wet_board(const std::string& board) const;
    bool is_paired_board(const std::string& board) const;
};

// Action tree explorer - recursively explores profitable paths
class ActionTreeExplorer {
public:
    ActionTreeExplorer(const AnalysisLimits& limits, MonteCarloEngine& engine);
    
    // Explore all profitable action trees for a hand/position/table combination
    std::vector<DiscoveredStrategy> explore_hand_position(
        const std::string& hand, 
        Position position, 
        int table_size
    );

private:
    const AnalysisLimits& limits_;
    MonteCarloEngine& engine_;
    DynamicBoardGenerator board_generator_;
    
    // Recursive tree exploration
    void explore_action_tree(
        const std::string& hand,
        Position position,
        std::vector<std::string> current_sequence,
        const std::string& board,
        Street street,
        int table_size,
        std::vector<DiscoveredStrategy>& discovered_strategies
    );
    
    // Test if an action is valid in current context
    bool is_valid_action(
        const std::string& action, 
        Street street, 
        const std::vector<std::string>& current_sequence
    ) const;
    
    // Find optimal frequency for an action
    DiscoveredStrategy test_action_frequencies(
        const std::string& hand,
        Position position,
        const std::vector<std::string>& action_sequence,
        const std::string& board,
        Street street,
        const std::string& action,
        int table_size
    );
    
    // Determine next street and board after action
    std::pair<Street, std::string> get_next_state(
        Street current_street,
        const std::string& current_board,
        const std::string& action
    ) const;
};

// Main dynamic analyzer class
class DynamicMonteCarloAnalyzer {
public:
    DynamicMonteCarloAnalyzer(const AnalysisLimits& limits = AnalysisLimits{});
    
    // Analyze within the defined limits
    void analyze_within_limits();
    
    // Analyze specific table size
    void analyze_table_size(int table_size);
    
    // Export discovered strategies to CSV
    void export_strategies_to_csv(int table_size) const;
    
    // Progress tracking
    int get_total_combinations() const { return total_combinations_; }
    int get_completed_combinations() const { return completed_combinations_; }
    
    // Get discovered strategies
    const std::vector<DiscoveredStrategy>& get_strategies(int table_size) const;
    
    // Configuration
    void set_limits(const AnalysisLimits& limits) { limits_ = limits; }
    const AnalysisLimits& get_limits() const { return limits_; }

private:
    AnalysisLimits limits_;
    MonteCarloEngine engine_;
    std::unique_ptr<ThreadPool> thread_pool_;
    
    DynamicHandGenerator hand_generator_;
    ActionTreeExplorer action_explorer_;
    
    // Results storage - only profitable strategies
    std::map<int, std::vector<DiscoveredStrategy>> discovered_strategies_;
    
    // Progress tracking
    std::atomic<int> total_combinations_;
    std::atomic<int> completed_combinations_;
    
    // Analysis functions
    void analyze_hand_position_combination(
        const std::string& hand,
        Position position,
        int table_size
    );
    
    // Utility functions
    void setup_thread_pool();
    void clear_strategies_for_table(int table_size);
    void print_progress(const std::string& message) const;
    std::string get_output_directory(int table_size) const;
    void create_output_directories() const;
    
    // Results filtering and aggregation
    void filter_profitable_strategies(int table_size);
    void remove_duplicate_strategies(int table_size);
    void sort_strategies_by_ev(int table_size);
};

} // namespace poker
