#include "decision_engine.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <iomanip>
#include <chrono>
#include <sstream>
#include <algorithm>
#include <random>

namespace poker {

// Static member definitions
const std::vector<double> DynamicStrategyEngine::FINE_GRAINED_FREQUENCIES = {
    0.00, 0.05, 0.10, 0.15, 0.20, 0.25, 0.30, 0.35, 0.40, 0.45, 0.50,
    0.55, 0.60, 0.65, 0.70, 0.75, 0.80, 0.85, 0.90, 0.95, 1.00
};

const int DynamicStrategyEngine::SIMULATIONS_PER_FREQUENCY = 50000;

// DecisionPoint methods
std::string DecisionPoint::get_unique_id() const {
    std::ostringstream oss;
    oss << hand << "_" << position_to_string(position) << "_" 
        << street_to_string(street) << "_" << decision_type << "_";
    
    // Add action history
    for (size_t i = 0; i < action_history.size(); ++i) {
        if (i > 0) oss << "-";
        oss << action_history[i];
    }
    
    oss << "_" << board << "_T" << table_size;
    return oss.str();
}

// BoardGenerator implementation
std::vector<std::string> BoardGenerator::generate_flop_boards() const {
    std::vector<std::string> boards;
    
    // Rainbow dry boards (different suits, no draws)
    std::vector<std::string> dry_rainbow = {
        "As7h2c", "Kd8s3h", "Qc9h4d", "Js6c2s", "Th5d3c",
        "9s4h2d", "8d5c3h", "7h3s2c", "6c4d2h", "5s3h2d"
    };
    
    // Two-tone dry boards (two suits, minimal draws)  
    std::vector<std::string> dry_two_tone = {
        "AhKc7h", "QsJd9s", "Tc8h5c", "9d7s4d", "8c6h3c",
        "7s5d2s", "6h4c2h", "5d3s2d", "AsQc8s", "KhJd7h"
    };
    
    // Wet coordinated boards (straight/flush draws)
    std::vector<std::string> wet_boards = {
        "9h8d7c", "8s7h6d", "7c6s5h", "6h5d4c", "5s4h3d",
        "Th9c8s", "Js9h8d", "Qc9s8h", "Kd9h8c", "AhTc9s",
        "9h8h7c", "8s7s6d", "7c6c5h", "6h5h4d", "5d4d3c"
    };
    
    // Paired boards
    std::vector<std::string> paired_boards = {
        "AaAh7c", "KkKd5s", "QqQc8h", "JjJs4d", "TtTc6s",
        "99Qc2h", "88Kd3s", "77Ah5c", "66Js9h", "55Tc7d",
        "44Ah8s", "33Kc9h", "22Qd7s"
    };
    
    // Flush draw boards (two of same suit + one different)
    std::vector<std::string> flush_draw_boards = {
        "AhKh9c", "QsJs8d", "TdJd7c", "9c8c4h", "8h7h2d",
        "KsQs5h", "JhTh6c", "9d8d3s", "7c6c4h", "6s5s2d"
    };
    
    // Monotone boards (all same suit - very wet)
    std::vector<std::string> monotone_boards = {
        "AhJh8h", "KsQs7s", "TdJd5d", "9c8c4c", "7h6h2h",
        "QhTh9h", "JsTs8s", "9d7d6d", "8c6c5c", "6h4h3h"
    };
    
    // Combine all board types
    boards.insert(boards.end(), dry_rainbow.begin(), dry_rainbow.end());
    boards.insert(boards.end(), dry_two_tone.begin(), dry_two_tone.end());
    boards.insert(boards.end(), wet_boards.begin(), wet_boards.end());
    boards.insert(boards.end(), paired_boards.begin(), paired_boards.end());
    boards.insert(boards.end(), flush_draw_boards.begin(), flush_draw_boards.end());
    boards.insert(boards.end(), monotone_boards.begin(), monotone_boards.end());
    
    return boards;
}

std::vector<std::string> BoardGenerator::generate_turn_boards() const {
    std::vector<std::string> flop_boards = generate_flop_boards();
    std::vector<std::string> turn_boards;
    
    // Sample turn cards to add to flop boards
    std::vector<std::string> turn_cards = {
        "2d", "3h", "4s", "5c", "6d", "7h", "8s", "9c", "Td", "Jh", "Qs", "Kc", "Ad"
    };
    
    // For computational efficiency, sample subset of flop boards
    std::vector<std::string> sample_flops = {
        "As7h2c", "Kd8s3h", "9h8d7c", "Kh9h6s", "AsKdQc",
        "AaAh7c", "887c2d", "AhKh9c", "Ah9c5s", "98h7s"
    };
    
    for (const auto& flop : sample_flops) {
        for (const auto& turn_card : turn_cards) {
            // Avoid duplicate cards
            if (flop.find(turn_card[0]) == std::string::npos || 
                flop.find(turn_card[1]) == std::string::npos) {
                turn_boards.push_back(flop + turn_card);
            }
        }
    }
    
    return turn_boards;
}

std::vector<std::string> BoardGenerator::generate_river_boards() const {
    std::vector<std::string> turn_boards = generate_turn_boards();
    std::vector<std::string> river_boards;
    
    // Sample river cards
    std::vector<std::string> river_cards = {
        "2c", "3d", "4h", "5s", "6c", "7d", "8h", "9s", "Tc", "Jd", "Qh", "Ks", "Ac"
    };
    
    // Sample subset of turn boards for efficiency
    std::vector<std::string> sample_turns;
    if (turn_boards.size() > 20) {
        // Take every nth turn board to get reasonable sample
        size_t step = turn_boards.size() / 20;
        for (size_t i = 0; i < turn_boards.size(); i += step) {
            sample_turns.push_back(turn_boards[i]);
        }
    } else {
        sample_turns = turn_boards;
    }
    
    for (const auto& turn : sample_turns) {
        for (const auto& river_card : river_cards) {
            // Avoid duplicate cards
            if (turn.find(river_card[0]) == std::string::npos || 
                turn.find(river_card[1]) == std::string::npos) {
                river_boards.push_back(turn + river_card);
            }
        }
    }
    
    return river_boards;
}

// DecisionPointGenerator implementation
DecisionPointGenerator::DecisionPointGenerator() {
}

std::vector<DecisionPoint> DecisionPointGenerator::generate_all_decisions(int table_size) const {
    std::vector<DecisionPoint> all_decisions;
    
    auto preflop_decisions = generate_preflop_decisions(table_size);
    auto postflop_decisions = generate_postflop_decisions(table_size);
    
    all_decisions.insert(all_decisions.end(), preflop_decisions.begin(), preflop_decisions.end());
    all_decisions.insert(all_decisions.end(), postflop_decisions.begin(), postflop_decisions.end());
    
    return all_decisions;
}

std::vector<DecisionPoint> DecisionPointGenerator::generate_preflop_decisions(int table_size) const {
    std::vector<DecisionPoint> decisions;
    auto all_hands = get_all_starting_hands();
    auto positions = get_positions_for_table_size(table_size);
    
    // Generate all preflop decision points
    for (const auto& hand : all_hands) {
        for (const auto& position : positions) {
            
            // First to act decisions (no action history)
            std::vector<std::string> first_actions = {"open", "fold"};
            for (const auto& action : first_actions) {
                DecisionPoint decision;
                decision.hand = hand;
                decision.position = position;
                decision.action_history = {};
                decision.board = "";
                decision.street = Street::PREFLOP;
                decision.decision_type = action;
                decision.table_size = table_size;
                decision.num_opponents = table_size - 1;
                decision.pot_size = 1.5; // SB + BB
                decision.bet_size = (action == "open") ? 2.5 : 0.0;
                
                decisions.push_back(decision);
            }
            
            // Facing open decisions
            std::vector<std::string> facing_open_actions = {"3bet", "call", "fold"};
            for (const auto& action : facing_open_actions) {
                DecisionPoint decision;
                decision.hand = hand;
                decision.position = position;
                decision.action_history = {"open"};
                decision.board = "";
                decision.street = Street::PREFLOP;
                decision.decision_type = action;
                decision.table_size = table_size;
                decision.num_opponents = table_size - 2; // Opener + us
                decision.pot_size = 4.0; // SB + BB + open
                decision.bet_size = (action == "3bet") ? 10.0 : (action == "call") ? 2.5 : 0.0;
                
                decisions.push_back(decision);
            }
            
            // Facing 3bet decisions
            std::vector<std::string> facing_3bet_actions = {"4bet", "call", "fold"};
            for (const auto& action : facing_3bet_actions) {
                DecisionPoint decision;
                decision.hand = hand;
                decision.position = position;
                decision.action_history = {"open", "3bet"};
                decision.board = "";
                decision.street = Street::PREFLOP;
                decision.decision_type = action;
                decision.table_size = table_size;
                decision.num_opponents = 1; // Just 3bettor
                decision.pot_size = 14.5; // SB + BB + open + 3bet
                decision.bet_size = (action == "4bet") ? 25.0 : (action == "call") ? 10.0 : 0.0;
                
                decisions.push_back(decision);
            }
            
            // Facing 4bet decisions
            std::vector<std::string> facing_4bet_actions = {"5bet", "call", "fold"};
            for (const auto& action : facing_4bet_actions) {
                DecisionPoint decision;
                decision.hand = hand;
                decision.position = position;
                decision.action_history = {"open", "3bet", "4bet"};
                decision.board = "";
                decision.street = Street::PREFLOP;
                decision.decision_type = action;
                decision.table_size = table_size;
                decision.num_opponents = 1;
                decision.pot_size = 39.5; // Previous + 4bet
                decision.bet_size = (action == "5bet") ? 60.0 : (action == "call") ? 25.0 : 0.0;
                
                decisions.push_back(decision);
            }
        }
    }
    
    return decisions;
}

std::vector<DecisionPoint> DecisionPointGenerator::generate_postflop_decisions(int table_size) const {
    std::vector<DecisionPoint> decisions;
    
    // Generate flop decisions
    auto flop_boards = board_generator_.generate_flop_boards();
    auto flop_decisions = generate_street_decisions(Street::FLOP, table_size, flop_boards);
    
    // Generate turn decisions  
    auto turn_boards = board_generator_.generate_turn_boards();
    auto turn_decisions = generate_street_decisions(Street::TURN, table_size, turn_boards);
    
    // Generate river decisions
    auto river_boards = board_generator_.generate_river_boards();
    auto river_decisions = generate_street_decisions(Street::RIVER, table_size, river_boards);
    
    decisions.insert(decisions.end(), flop_decisions.begin(), flop_decisions.end());
    decisions.insert(decisions.end(), turn_decisions.begin(), turn_decisions.end());
    decisions.insert(decisions.end(), river_decisions.begin(), river_decisions.end());
    
    return decisions;
}

std::vector<DecisionPoint> DecisionPointGenerator::generate_street_decisions(
    Street street, int table_size, const std::vector<std::string>& boards) const {
    
    std::vector<DecisionPoint> decisions;
    auto all_hands = get_all_starting_hands();
    auto positions = get_positions_for_table_size(table_size);
    auto action_histories = get_action_histories_for_street(street);
    
    for (const auto& hand : all_hands) {
        for (const auto& position : positions) {
            for (const auto& board : boards) {
                for (const auto& history : action_histories) {
                    auto available_actions = get_available_decisions(history, street);
                    
                    for (const auto& action : available_actions) {
                        DecisionPoint decision;
                        decision.hand = hand;
                        decision.position = position;
                        decision.action_history = history;
                        decision.board = board;
                        decision.street = street;
                        decision.decision_type = action;
                        decision.table_size = table_size;
                        decision.num_opponents = 1; // Simplified - heads up postflop
                        
                        // Set pot and bet sizes based on action history and decision
                        double base_pot = 10.0; // Typical preflop pot after open/call
                        decision.pot_size = base_pot * (1 + history.size() * 0.5); // Rough estimate
                        decision.bet_size = (action == "bet" || action == "cbet") ? decision.pot_size * 0.75 : 
                                          (action == "raise") ? decision.pot_size * 1.5 : 0.0;
                        
                        decisions.push_back(decision);
                    }
                }
            }
        }
    }
    
    return decisions;
}

std::vector<std::string> DecisionPointGenerator::get_all_starting_hands() const {
    return {
        // Pocket pairs
        "AA", "KK", "QQ", "JJ", "TT", "99", "88", "77", "66", "55", "44", "33", "22",
        
        // Suited hands
        "AKs", "AQs", "AJs", "ATs", "A9s", "A8s", "A7s", "A6s", "A5s", "A4s", "A3s", "A2s",
        "KQs", "KJs", "KTs", "K9s", "K8s", "K7s", "K6s", "K5s", "K4s", "K3s", "K2s",
        "QJs", "QTs", "Q9s", "Q8s", "Q7s", "Q6s", "Q5s", "Q4s", "Q3s", "Q2s",
        "JTs", "J9s", "J8s", "J7s", "J6s", "J5s", "J4s", "J3s", "J2s",
        "T9s", "T8s", "T7s", "T6s", "T5s", "T4s", "T3s", "T2s",
        "98s", "97s", "96s", "95s", "94s", "93s", "92s",
        "87s", "86s", "85s", "84s", "83s", "82s",
        "76s", "75s", "74s", "73s", "72s",
        "65s", "64s", "63s", "62s",
        "54s", "53s", "52s",
        "43s", "42s",
        "32s",
        
        // Offsuit hands
        "AKo", "AQo", "AJo", "ATo", "A9o", "A8o", "A7o", "A6o", "A5o", "A4o", "A3o", "A2o",
        "KQo", "KJo", "KTo", "K9o", "K8o", "K7o", "K6o", "K5o", "K4o", "K3o", "K2o",
        "QJo", "QTo", "Q9o", "Q8o", "Q7o", "Q6o", "Q5o", "Q4o", "Q3o", "Q2o",
        "JTo", "J9o", "J8o", "J7o", "J6o", "J5o", "J4o", "J3o", "J2o",
        "T9o", "T8o", "T7o", "T6o", "T5o", "T4o", "T3o", "T2o",
        "98o", "97o", "96o", "95o", "94o", "93o", "92o",
        "87o", "86o", "85o", "84o", "83o", "82o",
        "76o", "75o", "74o", "73o", "72o",
        "65o", "64o", "63o", "62o",
        "54o", "53o", "52o",
        "43o", "42o",
        "32o"
    };
}

std::vector<Position> DecisionPointGenerator::get_positions_for_table_size(int table_size) const {
    if (table_size == 6) {
        return {Position::UTG, Position::HJ, Position::CO, Position::BTN, Position::SB, Position::BB};
    } else if (table_size == 9) {
        // For 9-max, we'll still use 6 positions but they represent different ranges
        return {Position::UTG, Position::HJ, Position::CO, Position::BTN, Position::SB, Position::BB};
    } else {
        // 7-max and 8-max use same positions
        return {Position::UTG, Position::HJ, Position::CO, Position::BTN, Position::SB, Position::BB};
    }
}

std::vector<std::vector<std::string>> DecisionPointGenerator::get_action_histories_for_street(Street street) const {
    switch (street) {
        case Street::FLOP:
            return {
                {"open", "call"},           // Standard continuation
                {"open", "3bet", "call"},   // 3bet pot
                {"open", "call", "check"},  // Checked to us
            };
        case Street::TURN:
            return {
                {"open", "call", "cbet", "call"},
                {"open", "call", "check", "check"},
                {"open", "3bet", "call", "cbet", "call"},
            };
        case Street::RIVER:
            return {
                {"open", "call", "cbet", "call", "bet", "call"},
                {"open", "call", "cbet", "call", "check", "check"},
                {"open", "call", "check", "check", "check", "check"},
            };
        default:
            return {{}};
    }
}

std::vector<std::string> DecisionPointGenerator::get_available_decisions(
    const std::vector<std::string>& action_history, Street street) const {
    
    if (action_history.empty() || action_history.back() == "check") {
        // First to act or facing check
        return {"bet", "check"};
    } else if (action_history.back() == "bet" || action_history.back() == "cbet") {
        // Facing bet
        return {"raise", "call", "fold"};
    } else if (action_history.back() == "raise") {
        // Facing raise
        return {"3bet", "call", "fold"};
    }
    
    // Default actions
    return {"bet", "check", "call", "fold"};
}

// DynamicStrategyEngine implementation
DynamicStrategyEngine::DynamicStrategyEngine() : total_decisions_(0), completed_decisions_(0) {
}

void DynamicStrategyEngine::discover_all_strategies() {
    create_output_directories();
    
    for (int table_size = 6; table_size <= 9; ++table_size) {
        std::cout << "\n=== Discovering optimal strategy for " << table_size << "-max games ===" << std::endl;
        discover_strategy_for_table_size(table_size);
    }
    
    std::cout << "\nAll strategy discovery complete!" << std::endl;
}

void DynamicStrategyEngine::discover_strategy_for_table_size(int table_size) {
    clear_results_for_table(table_size);
    
    // Generate all decision points for this table size
    auto all_decisions = decision_generator_.generate_all_decisions(table_size);
    
    total_decisions_ = static_cast<int>(all_decisions.size());
    completed_decisions_ = 0;
    
    print_progress("Starting " + std::to_string(table_size) + "-max strategy discovery...");
    print_progress("Total decision points to analyze: " + std::to_string(total_decisions_));
    
    std::vector<DecisionResult> results;
    
    for (const auto& decision : all_decisions) {
        if (is_valid_decision_point(decision)) {
            DecisionResult result = test_decision_point(decision);
            results.push_back(result);
        }
        
        completed_decisions_++;
        
        if (completed_decisions_ % 1000 == 0) {
            print_progress("Completed " + std::to_string(completed_decisions_) + "/" + 
                          std::to_string(total_decisions_) + " decisions");
        }
    }
    
    // Store results
    table_results_[table_size] = std::move(results);
    
    // Export to CSV
    export_strategies_to_csv(table_size);
    
    print_progress("Strategy discovery complete for " + std::to_string(table_size) + "-max!");
}

DecisionResult DynamicStrategyEngine::test_decision_point(const DecisionPoint& decision) {
    return find_optimal_frequency_precise(decision);
}

DecisionResult DynamicStrategyEngine::find_optimal_frequency_precise(const DecisionPoint& decision) {
    DecisionResult result;
    result.decision = decision;
    result.test_frequencies = FINE_GRAINED_FREQUENCIES;
    result.frequency_evs.resize(FINE_GRAINED_FREQUENCIES.size());
    
    double best_ev = -999.0;
    double best_frequency = 0.0;
    
    // Create scenario for Monte Carlo testing
    Scenario scenario;
    scenario.hand = decision.hand;
    scenario.position = decision.position;
    scenario.actions = decision.action_history;
    scenario.actions.push_back(decision.decision_type); // Add current decision
    scenario.board = decision.board;
    scenario.street = decision.street;
    scenario.table_size = decision.table_size;
    scenario.num_opponents = decision.num_opponents;
    
    // Test each frequency
    for (size_t i = 0; i < FINE_GRAINED_FREQUENCIES.size(); ++i) {
        double frequency = FINE_GRAINED_FREQUENCIES[i];
        double ev = monte_carlo_.test_frequency(scenario, frequency, SIMULATIONS_PER_FREQUENCY);
        
        result.frequency_evs[i] = ev;
        
        if (ev > best_ev) {
            best_ev = ev;
            best_frequency = frequency;
        }
    }
    
    result.optimal_frequency = best_frequency;
    result.max_expected_value = best_ev;
    result.total_simulations = SIMULATIONS_PER_FREQUENCY * static_cast<int>(FINE_GRAINED_FREQUENCIES.size());
    result.is_profitable = (best_ev > 0.0);
    
    return result;
}

void DynamicStrategyEngine::export_strategies_to_csv(int table_size) {
    std::string output_dir = get_output_directory(table_size);
    
    auto& results = table_results_[table_size];
    
    // Separate results by street for easier analysis
    std::vector<DecisionResult> preflop_results;
    std::vector<DecisionResult> flop_results;
    std::vector<DecisionResult> turn_results;
    std::vector<DecisionResult> river_results;
    
    for (const auto& result : results) {
        switch (result.decision.street) {
            case Street::PREFLOP:
                preflop_results.push_back(result);
                break;
            case Street::FLOP:
                flop_results.push_back(result);
                break;
            case Street::TURN:
                turn_results.push_back(result);
                break;
            case Street::RIVER:
                river_results.push_back(result);
                break;
        }
    }
    
    // Export function for each street
    auto export_street_results = [&](const std::vector<DecisionResult>& street_results, 
                                     const std::string& filename) {
        std::ofstream file(output_dir + "/" + filename);
        if (!file.is_open()) {
            std::cerr << "Failed to create file: " << filename << std::endl;
            return;
        }
        
        // Write CSV header
        file << "table_size,hand,position,street,board,action_history,decision_type,"
             << "optimal_frequency,max_expected_value,is_profitable,total_simulations,"
             << "pot_size,bet_size,decision_id\n";
        
        // Write data
        for (const auto& result : street_results) {
            const DecisionPoint& d = result.decision;
            
            // Convert action history to string
            std::string history_str;
            for (size_t i = 0; i < d.action_history.size(); ++i) {
                if (i > 0) history_str += "-";
                history_str += d.action_history[i];
            }
            
            file << std::fixed << std::setprecision(6)
                 << d.table_size << ","
                 << d.hand << ","
                 << position_to_string(d.position) << ","
                 << street_to_string(d.street) << ","
                 << d.board << ","
                 << history_str << ","
                 << d.decision_type << ","
                 << result.optimal_frequency << ","
                 << result.max_expected_value << ","
                 << (result.is_profitable ? "TRUE" : "FALSE") << ","
                 << result.total_simulations << ","
                 << d.pot_size << ","
                 << d.bet_size << ","
                 << d.get_unique_id() << "\n";
        }
        
        file.close();
        std::cout << "Exported " << street_results.size() << " results to " << filename << std::endl;
    };
    
    export_street_results(preflop_results, "dynamic_preflop_strategy.csv");
    export_street_results(flop_results, "dynamic_flop_strategy.csv");
    export_street_results(turn_results, "dynamic_turn_strategy.csv");
    export_street_results(river_results, "dynamic_river_strategy.csv");
    
    // Also create a summary file with only profitable decisions
    std::vector<DecisionResult> profitable_results;
    for (const auto& result : results) {
        if (result.is_profitable) {
            profitable_results.push_back(result);
        }
    }
    
    export_street_results(profitable_results, "profitable_strategies_only.csv");
}

bool DynamicStrategyEngine::is_valid_decision_point(const DecisionPoint& decision) const {
    // Basic validation
    if (decision.hand.empty() || decision.decision_type.empty()) {
        return false;
    }
    
    // Don't test fold decisions (they're always EV=0)
    if (decision.decision_type == "fold") {
        return false;
    }
    
    // For computational efficiency, limit some combinations
    if (decision.street != Street::PREFLOP && decision.hand == "72o") {
        return false; // Skip worst hand postflop
    }
    
    return true;
}

void DynamicStrategyEngine::clear_results_for_table(int table_size) {
    table_results_[table_size].clear();
    total_decisions_ = 0;
    completed_decisions_ = 0;
}

void DynamicStrategyEngine::print_progress(const std::string& message) {
    auto now = std::chrono::steady_clock::now();
    auto duration = now.time_since_epoch();
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
    
    std::cout << "[" << seconds << "] " << message << std::endl;
}

std::string DynamicStrategyEngine::get_output_directory(int table_size) {
    return "output/dynamic_" + std::to_string(table_size) + "max";
}

void DynamicStrategyEngine::create_output_directories() {
    for (int table_size = 6; table_size <= 9; ++table_size) {
        std::string dir = get_output_directory(table_size);
        std::filesystem::create_directories(dir);
    }
}

} // namespace poker
