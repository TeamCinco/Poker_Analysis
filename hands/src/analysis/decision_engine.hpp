#pragma once

#include "../core/types.hpp"
#include "../engine/monte_carlo_engine.hpp"
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>

namespace poker {

// Represents a single decision point in poker
struct DecisionPoint {
    std::string hand;                           // "AA", "72o", etc.
    Position position;                          // UTG, BTN, etc.
    std::vector<std::string> action_history;    // What happened before this decision
    std::string board;                          // Current board (empty for preflop)
    Street street;                             // preflop, flop, turn, river
    std::string decision_type;                 // "open", "3bet", "cbet", "barrel", etc.
    int table_size;                           // 6, 7, 8, or 9
    int num_opponents;                        // Remaining opponents
    double pot_size;                          // Current pot size
    double bet_size;                          // Size of bet/raise being considered
    
    std::string get_unique_id() const;
};

// Stores the result of testing a decision point
struct DecisionResult {
    DecisionPoint decision;
    double optimal_frequency;          // 0.0 to 1.0 
    double max_expected_value;         // EV at optimal frequency
    std::vector<double> frequency_evs; // EV at each tested frequency
    std::vector<double> test_frequencies; // Frequencies that were tested
    int total_simulations;
    bool is_profitable;               // True if max_ev > 0
    
    DecisionResult() : optimal_frequency(0.0), max_expected_value(-999.0), 
                      total_simulations(0), is_profitable(false) {}
};

// Generates all possible boards systematically
class BoardGenerator {
public:
    // Generate representative flop boards covering all texture types
    std::vector<std::string> generate_flop_boards() const;
    
    // Generate turn boards by adding turn cards to flop boards
    std::vector<std::string> generate_turn_boards() const;
    
    // Generate river boards by adding river cards to turn boards  
    std::vector<std::string> generate_river_boards() const;
    
private:
    // Board texture classification helpers
    bool is_dry_board(const std::string& board) const;
    bool is_wet_board(const std::string& board) const;
    bool is_paired_board(const std::string& board) const;
    bool has_flush_draw(const std::string& board) const;
    bool has_straight_draw(const std::string& board) const;
};

// Generates all possible decision points dynamically  
class DecisionPointGenerator {
public:
    DecisionPointGenerator();
    
    // Generate all decision points for a given table size
    std::vector<DecisionPoint> generate_all_decisions(int table_size) const;
    
    // Generate preflop decision points
    std::vector<DecisionPoint> generate_preflop_decisions(int table_size) const;
    
    // Generate postflop decision points
    std::vector<DecisionPoint> generate_postflop_decisions(int table_size) const;
    
private:
    BoardGenerator board_generator_;
    
    // All 169 starting hands
    std::vector<std::string> get_all_starting_hands() const;
    
    // All positions for given table size
    std::vector<Position> get_positions_for_table_size(int table_size) const;
    
    // Generate decision points for specific street
    std::vector<DecisionPoint> generate_street_decisions(
        Street street, int table_size, const std::vector<std::string>& boards) const;
    
    // Get all possible action histories leading to current street
    std::vector<std::vector<std::string>> get_action_histories_for_street(Street street) const;
    
    // Get decision types available at current decision point
    std::vector<std::string> get_available_decisions(
        const std::vector<std::string>& action_history, Street street) const;
};

// Main class that discovers optimal strategies dynamically
class DynamicStrategyEngine {
public:
    DynamicStrategyEngine();
    
    // Discover optimal strategies for all table sizes
    void discover_all_strategies();
    
    // Discover strategy for specific table size
    void discover_strategy_for_table_size(int table_size);
    
    // Test a single decision point and find optimal frequency
    DecisionResult test_decision_point(const DecisionPoint& decision);
    
    // Export discovered strategies to CSV
    void export_strategies_to_csv(int table_size);
    
    // Get progress information
    int get_total_decisions() const { return total_decisions_; }
    int get_completed_decisions() const { return completed_decisions_; }

private:
    DecisionPointGenerator decision_generator_;
    MonteCarloEngine monte_carlo_;
    
    // Results storage
    std::unordered_map<int, std::vector<DecisionResult>> table_results_; // table_size -> results
    
    // Progress tracking
    int total_decisions_;
    int completed_decisions_;
    
    // Configuration
    static const std::vector<double> FINE_GRAINED_FREQUENCIES; // 21 frequencies: 0%, 5%, 10%, ..., 100%
    static const int SIMULATIONS_PER_FREQUENCY;
    
    // Helper functions
    void clear_results_for_table(int table_size);
    void print_progress(const std::string& message);
    std::string get_output_directory(int table_size);
    void create_output_directories();
    
    // Find optimal frequency using more precise search
    DecisionResult find_optimal_frequency_precise(const DecisionPoint& decision);
    
    // Validate that a decision point makes sense
    bool is_valid_decision_point(const DecisionPoint& decision) const;
};

} // namespace poker
