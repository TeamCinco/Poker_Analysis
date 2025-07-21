#pragma once

#include "../core/types.hpp"
#include "../core/deck.hpp"
#include "../core/hand_evaluator.hpp"
#include <random>
#include <vector>

namespace poker {

class MonteCarloEngine {
public:
    MonteCarloEngine();
    
    // Main simulation function - tests a scenario at a specific frequency
    SimResult simulate_scenario(const Scenario& scenario);
    
    // Test a specific frequency for a scenario (0.0 to 1.0)
    double test_frequency(const Scenario& scenario, double frequency, int num_simulations = 100000);
    
    // Find optimal frequency by testing 0%, 25%, 50%, 75%, 100%
    SimResult find_optimal_frequency(const Scenario& scenario, int num_simulations = 100000);

private:
    std::mt19937 rng_;
    
    // Test frequencies to try
    static const std::vector<double> TEST_FREQUENCIES;
    
    // Core simulation functions
    double simulate_preflop_scenario(const Scenario& scenario, double frequency, int num_simulations);
    double simulate_postflop_scenario(const Scenario& scenario, double frequency, int num_simulations);
    
    // Single hand simulation
    double simulate_single_hand(const std::array<Card, 2>& hero_hand,
                               const std::vector<Card>& board,
                               int num_opponents,
                               const std::string& action,
                               double frequency);
    
    // Deal opponent hands
    std::vector<std::array<Card, 2>> deal_opponent_hands(Deck& deck, int num_opponents);
    
    // Calculate equity for a hand vs opponents
    double calculate_equity(const std::array<Card, 2>& hero_hand,
                          const std::vector<std::array<Card, 2>>& opponent_hands,
                          const std::vector<Card>& board);
    
    // Simulate betting action outcome
    double simulate_action_outcome(const std::string& action, double equity, double pot_odds = 2.0);
    
    // Parse starting hand notation (AA, KQs, 72o, etc.) to actual cards
    std::array<Card, 2> parse_starting_hand(const std::string& hand_str);
};

} // namespace poker
