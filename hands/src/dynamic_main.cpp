#include "analysis/dynamic_monte_carlo_analyzer.hpp"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    std::cout << "=== DYNAMIC BOUNDARY-DRIVEN MONTE CARLO POKER ANALYZER ===" << std::endl;
    std::cout << "Pure Mathematical Discovery - No Hardcoded Strategies!" << std::endl;
    std::cout << "Only boundary parameters + Monte Carlo exploration within limits" << std::endl;
    std::cout << std::endl;
    
    // Check command line arguments
    if (argc > 1) {
        std::string arg = argv[1];
        if (arg == "--help" || arg == "-h") {
            std::cout << "Usage: " << argv[0] << " [table_size] [--fast]" << std::endl;
            std::cout << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  No arguments: Discover strategies for all table sizes (6-max through 9-max)" << std::endl;
            std::cout << "  6, 7, 8, 9:   Discover strategy for specific table size only" << std::endl;
            std::cout << "  --fast:       Use reduced simulation count for faster analysis" << std::endl;
            std::cout << "  --help, -h:   Show this help message" << std::endl;
            std::cout << std::endl;
            std::cout << "BOUNDARY-DRIVEN APPROACH:" << std::endl;
            std::cout << "• Set boundary parameters (ranks, positions, actions, board samples)" << std::endl;
            std::cout << "• Generate ALL possible combinations within those boundaries" << std::endl;
            std::cout << "• Test each combination at multiple frequencies (0% to 100%)" << std::endl;
            std::cout << "• ONLY keep strategies with positive expected value (EV > 0.01)" << std::endl;
            std::cout << "• Recursively explore profitable action trees" << std::endl;
            std::cout << std::endl;
            std::cout << "OUTPUT STRUCTURE:" << std::endl;
            std::cout << "  output/dynamic_6max/dynamic_preflop_strategy.csv" << std::endl;
            std::cout << "  output/dynamic_6max/dynamic_flop_strategy.csv" << std::endl;
            std::cout << "  output/dynamic_6max/dynamic_turn_strategy.csv" << std::endl;
            std::cout << "  output/dynamic_6max/dynamic_river_strategy.csv" << std::endl;
            std::cout << "  output/dynamic_6max/profitable_strategies_only.csv" << std::endl;
            std::cout << "  ... (similar for 7max, 8max, 9max)" << std::endl;
            std::cout << std::endl;
            std::cout << "DYNAMIC DISCOVERY EXAMPLE:" << std::endl;
            std::cout << "• AA from UTG: Tests open at 0%, 10%, 20%...100%" << std::endl;
            std::cout << "• Finds 90% frequency has highest EV = +0.185 BB" << std::endl;
            std::cout << "• Saves: 'AA,UTG,preflop,open,0.90,0.185'" << std::endl;
            std::cout << "• 72o from UTG: Tests all frequencies, best EV = -0.250 BB" << std::endl;
            std::cout << "• Rejects: EV < 0.01 threshold, not saved" << std::endl;
            return 0;
        }
    }
    
    try {
        // Configure analysis limits based on command line options
        poker::AnalysisLimits limits;
        
        // Check for fast mode
        bool fast_mode = false;
        for (int i = 1; i < argc; ++i) {
            if (std::string(argv[i]) == "--fast") {
                fast_mode = true;
                break;
            }
        }
        
        if (fast_mode) {
            std::cout << "🚀 FAST MODE ENABLED" << std::endl;
            limits.simulations_per_test = 10000;  // Reduced for speed
            limits.max_board_samples = 15;        // Fewer board samples
            limits.test_frequencies = {0.0, 0.2, 0.4, 0.6, 0.8, 1.0}; // Fewer frequency tests
            limits.max_action_depth = 4;          // Shallower action trees
            std::cout << "• Simulations per test: " << limits.simulations_per_test << std::endl;
            std::cout << "• Board samples: " << limits.max_board_samples << std::endl;
            std::cout << "• Frequency tests: " << limits.test_frequencies.size() << std::endl;
            std::cout << "• Max action depth: " << limits.max_action_depth << std::endl;
            std::cout << std::endl;
        } else {
            std::cout << "🔬 FULL ANALYSIS MODE" << std::endl;
            std::cout << "• Simulations per test: " << limits.simulations_per_test << std::endl;
            std::cout << "• Board samples: " << limits.max_board_samples << std::endl;
            std::cout << "• Frequency tests: " << limits.test_frequencies.size() << std::endl;
            std::cout << "• Max action depth: " << limits.max_action_depth << std::endl;
            std::cout << "⏰ Expected runtime: 20-45 minutes for comprehensive analysis" << std::endl;
            std::cout << std::endl;
        }
        
        // Display boundary parameters
        std::cout << "📊 BOUNDARY PARAMETERS:" << std::endl;
        std::cout << "• Ranks: ";
        for (size_t i = 0; i < limits.rank_range.size(); ++i) {
            std::cout << limits.rank_range[i];
            if (i < limits.rank_range.size() - 1) std::cout << ", ";
        }
        std::cout << " (" << limits.rank_range.size() << " ranks)" << std::endl;
        
        std::cout << "• Suit combinations: ";
        for (size_t i = 0; i < limits.suit_combinations.size(); ++i) {
            std::cout << limits.suit_combinations[i];
            if (i < limits.suit_combinations.size() - 1) std::cout << ", ";
        }
        std::cout << std::endl;
        
        std::cout << "• Positions: ";
        for (size_t i = 0; i < limits.positions.size(); ++i) {
            std::cout << poker::position_to_string(limits.positions[i]);
            if (i < limits.positions.size() - 1) std::cout << ", ";
        }
        std::cout << " (" << limits.positions.size() << " positions)" << std::endl;
        
        std::cout << "• Actions: ";
        for (size_t i = 0; i < limits.actions.size(); ++i) {
            std::cout << limits.actions[i];
            if (i < limits.actions.size() - 1) std::cout << ", ";
        }
        std::cout << " (" << limits.actions.size() << " actions)" << std::endl;
        
        std::cout << "• Minimum profitable EV: " << limits.min_profitable_ev << " BB" << std::endl;
        std::cout << std::endl;
        
        // Initialize dynamic analyzer with configured limits
        poker::DynamicMonteCarloAnalyzer analyzer(limits);
        
        if (argc > 1 && !fast_mode) {
            // Analyze specific table size
            int table_size = std::stoi(argv[1]);
            
            if (table_size < 6 || table_size > 9) {
                std::cerr << "Error: Table size must be 6, 7, 8, or 9" << std::endl;
                return 1;
            }
            
            std::cout << "🎯 ANALYZING " << table_size << "-MAX WITHIN BOUNDARIES" << std::endl;
            std::cout << "Dynamic exploration of all profitable strategies..." << std::endl;
            std::cout << std::endl;
            
            analyzer.analyze_table_size(table_size);
            
            // Display results summary
            const auto& strategies = analyzer.get_strategies(table_size);
            std::cout << std::endl;
            std::cout << "✅ BOUNDARY EXPLORATION COMPLETE!" << std::endl;
            std::cout << "📈 Discovered " << strategies.size() << " profitable strategies for " 
                      << table_size << "-max" << std::endl;
            
        } else {
            // Analyze all table sizes within boundaries
            std::cout << "🌟 COMPLETE BOUNDARY EXPLORATION" << std::endl;
            std::cout << "Discovering optimal strategies for all table sizes..." << std::endl;
            std::cout << "This explores EVERY combination within the defined limits!" << std::endl;
            std::cout << std::endl;
            
            analyzer.analyze_within_limits();
            
            // Display results summary for all table sizes
            std::cout << std::endl;
            std::cout << "🎉 COMPLETE BOUNDARY EXPLORATION FINISHED!" << std::endl;
            
            for (int table_size : limits.table_sizes) {
                const auto& strategies = analyzer.get_strategies(table_size);
                std::cout << "📊 " << table_size << "-max: " << strategies.size() 
                          << " profitable strategies discovered" << std::endl;
            }
        }
        
        std::cout << std::endl;
        std::cout << "💡 KEY INSIGHTS FROM BOUNDARY EXPLORATION:" << std::endl;
        std::cout << "• NO hardcoded strategies - pure mathematical discovery" << std::endl;
        std::cout << "• ONLY profitable plays survive (EV > " << limits.min_profitable_ev << ")" << std::endl;
        std::cout << "• Optimal frequencies found through Monte Carlo testing" << std::endl;
        std::cout << "• Dynamic action tree exploration - paths with +EV continue" << std::endl;
        std::cout << "• Board texture variations captured systematically" << std::endl;
        std::cout << std::endl;
        
        std::cout << "📁 RESULTS LOCATION:" << std::endl;
        std::cout << "- output/dynamic_6max/" << std::endl;
        std::cout << "- output/dynamic_7max/" << std::endl;
        std::cout << "- output/dynamic_8max/" << std::endl;
        std::cout << "- output/dynamic_9max/" << std::endl;
        std::cout << std::endl;
        
        std::cout << "📋 USE THESE FILES:" << std::endl;
        std::cout << "• profitable_strategies_only.csv - Your core playbook (ONLY +EV plays)" << std::endl;
        std::cout << "• dynamic_preflop_strategy.csv  - Opening, 3betting, calling ranges" << std::endl;
        std::cout << "• dynamic_flop_strategy.csv     - C-betting, calling frequencies" << std::endl;
        std::cout << "• dynamic_turn_strategy.csv     - Barreling and value betting" << std::endl;
        std::cout << "• dynamic_river_strategy.csv    - Final decision optimization" << std::endl;
        std::cout << std::endl;
        
        std::cout << "🚀 THE MATH HAS DISCOVERED YOUR OPTIMAL STRATEGY!" << std::endl;
        std::cout << "No assumptions, no poker 'theory' - just profitable plays within your boundaries." << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred during boundary exploration" << std::endl;
        return 1;
    }
    
    return 0;
}
