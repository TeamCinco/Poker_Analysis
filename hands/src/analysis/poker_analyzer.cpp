#include "poker_analyzer.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <iomanip>
#include <chrono>

namespace poker {

// Static member definitions
const std::vector<std::string> PokerAnalyzer::ALL_HANDS = {
    // Pocket pairs
    "AA", "KK", "QQ", "JJ", "TT", "99", "88", "77", "66", "55", "44", "33", "22",
    
    // Suited connectors and one-gappers
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

const std::vector<Position> PokerAnalyzer::ALL_POSITIONS = {
    Position::UTG, Position::HJ, Position::CO, Position::BTN, Position::SB, Position::BB
};

const std::vector<std::string> PokerAnalyzer::PREFLOP_ACTIONS = {
    "open", "3bet", "4bet", "5bet", "call", "fold"
};

const std::vector<std::string> PokerAnalyzer::POSTFLOP_ACTIONS = {
    "bet", "check", "raise", "call", "fold", "cbet"
};

const std::vector<std::string> PokerAnalyzer::SAMPLE_BOARDS = {
    // Dry boards
    "As7h2c", "Kd8s3h", "Qc9h4d", "Js6c2s", "Th5d3c",
    
    // Wet boards
    "9h8d7c", "Kh9h6s", "AsKdQc", "JsTc9d", "8c7s6h",
    
    // Paired boards  
    "AaAh7c", "KkKd5s", "QqQc8h", "JjJs4d", "TtTc3s",
    "887c2d", "776h4s", "665d9c", "554s8h", "443c7d",
    
    // Flush boards
    "AhKh9h", "QsJs8s", "TdJd7d", "9c6c3c", "8h5h2h",
    
    // Straight boards
    "9876", "8765", "7654", "6543", "5432",
    
    // Rainbow boards
    "Ah9c5s", "Kd7h3c", "Qs8d4h", "Jc6s2d", "Tc4h9s",
    
    // Two-tone boards
    "AhKh7c", "QsJs9d", "TdTc6h", "98h7s", "76s5c",
    
    // Monotone boards  
    "AhJh8h", "KsQs7s", "TdJd5d", "9c8c4c", "7h6h2h"
};

PokerAnalyzer::PokerAnalyzer() : total_scenarios_(0), completed_scenarios_(0) {
}

void PokerAnalyzer::analyze_all_table_sizes() {
    create_output_directories();
    
    for (int table_size = 6; table_size <= 9; ++table_size) {
        std::cout << "\n=== Analyzing " << table_size << "-max games ===" << std::endl;
        analyze_table_size(table_size);
    }
    
    std::cout << "\nAll analysis complete!" << std::endl;
}

void PokerAnalyzer::analyze_table_size(int table_size) {
    clear_results();
    
    // Count total scenarios for progress tracking
    auto preflop_scenarios = generate_preflop_scenarios(table_size);
    auto flop_scenarios = generate_flop_scenarios(table_size);
    auto turn_scenarios = generate_turn_scenarios(table_size);  
    auto river_scenarios = generate_river_scenarios(table_size);
    
    total_scenarios_ = static_cast<int>(preflop_scenarios.size() + flop_scenarios.size() + 
                                      turn_scenarios.size() + river_scenarios.size());
    completed_scenarios_ = 0;
    
    print_progress("Starting " + std::to_string(table_size) + "-max analysis...");
    
    // Analyze preflop scenarios
    print_progress("Analyzing preflop scenarios...");
    for (const auto& scenario : preflop_scenarios) {
        SimResult result = engine_.simulate_scenario(scenario);
        results_.emplace_back(scenario, result);
        completed_scenarios_++;
        
        if (completed_scenarios_ % 100 == 0) {
            print_progress("Completed " + std::to_string(completed_scenarios_) + "/" + 
                          std::to_string(total_scenarios_) + " scenarios");
        }
    }
    
    // Analyze flop scenarios
    print_progress("Analyzing flop scenarios...");
    for (const auto& scenario : flop_scenarios) {
        SimResult result = engine_.simulate_scenario(scenario);
        results_.emplace_back(scenario, result);
        completed_scenarios_++;
        
        if (completed_scenarios_ % 100 == 0) {
            print_progress("Completed " + std::to_string(completed_scenarios_) + "/" + 
                          std::to_string(total_scenarios_) + " scenarios");
        }
    }
    
    // Analyze turn scenarios  
    print_progress("Analyzing turn scenarios...");
    for (const auto& scenario : turn_scenarios) {
        SimResult result = engine_.simulate_scenario(scenario);
        results_.emplace_back(scenario, result);
        completed_scenarios_++;
        
        if (completed_scenarios_ % 100 == 0) {
            print_progress("Completed " + std::to_string(completed_scenarios_) + "/" + 
                          std::to_string(total_scenarios_) + " scenarios");
        }
    }
    
    // Analyze river scenarios
    print_progress("Analyzing river scenarios...");
    for (const auto& scenario : river_scenarios) {
        SimResult result = engine_.simulate_scenario(scenario);
        results_.emplace_back(scenario, result);
        completed_scenarios_++;
        
        if (completed_scenarios_ % 100 == 0) {
            print_progress("Completed " + std::to_string(completed_scenarios_) + "/" + 
                          std::to_string(total_scenarios_) + " scenarios");
        }
    }
    
    // Export results
    export_results_to_csv(table_size);
    print_progress("Analysis complete for " + std::to_string(table_size) + "-max!");
}

std::vector<Scenario> PokerAnalyzer::generate_preflop_scenarios(int table_size) {
    std::vector<Scenario> scenarios;
    
    // Generate scenarios for each hand and position
    for (const auto& hand : ALL_HANDS) {
        for (const auto& position : ALL_POSITIONS) {
            for (const auto& action : PREFLOP_ACTIONS) {
                Scenario scenario;
                scenario.hand = hand;
                scenario.position = position;
                scenario.actions = {action};
                scenario.board = "";
                scenario.street = Street::PREFLOP;
                scenario.table_size = table_size;
                scenario.num_opponents = table_size - 1;
                
                scenarios.push_back(scenario);
            }
            
            // Multi-action sequences
            std::vector<std::vector<std::string>> multi_actions = {
                {"open", "3bet"},
                {"open", "call"},
                {"3bet", "4bet"},
                {"3bet", "call"},
                {"4bet", "5bet"},
                {"4bet", "call"}
            };
            
            for (const auto& action_sequence : multi_actions) {
                Scenario scenario;
                scenario.hand = hand;
                scenario.position = position;
                scenario.actions = action_sequence;
                scenario.board = "";
                scenario.street = Street::PREFLOP;
                scenario.table_size = table_size;
                scenario.num_opponents = table_size - 1;
                
                scenarios.push_back(scenario);
            }
        }
    }
    
    return scenarios;
}

std::vector<Scenario> PokerAnalyzer::generate_flop_scenarios(int table_size) {
    std::vector<Scenario> scenarios;
    
    // Subset of hands for postflop (premium hands + some speculative hands)
    std::vector<std::string> postflop_hands = {
        "AA", "KK", "QQ", "JJ", "TT", "99", "88", "77",
        "AKs", "AKo", "AQs", "AQo", "AJs", "AJo", "ATs", "ATo",
        "KQs", "KQo", "KJs", "KJo", "QJs", "QJo", "JTs", "T9s",
        "98s", "87s", "76s", "65s", "54s"
    };
    
    for (const auto& hand : postflop_hands) {
        for (const auto& position : ALL_POSITIONS) {
            for (const auto& board : SAMPLE_BOARDS) {
                for (const auto& action : POSTFLOP_ACTIONS) {
                    Scenario scenario;
                    scenario.hand = hand;
                    scenario.position = position;
                    scenario.actions = {"open", "call", action}; // Previous preflop action + flop action
                    scenario.board = board;
                    scenario.street = Street::FLOP;
                    scenario.table_size = table_size;
                    scenario.num_opponents = table_size - 1;
                    
                    scenarios.push_back(scenario);
                }
            }
        }
    }
    
    return scenarios;
}

std::vector<Scenario> PokerAnalyzer::generate_turn_scenarios(int table_size) {
    std::vector<Scenario> scenarios;
    
    // Even smaller subset for turn analysis
    std::vector<std::string> turn_hands = {
        "AA", "KK", "QQ", "JJ", "AKs", "AKo", "AQs", "KQs", "JTs", "98s"
    };
    
    // Sample turn boards (flop + turn card)
    std::vector<std::string> turn_boards = {
        "As7h2cKd", "Kd8s3hQc", "9h8d7c6s", "Kh9h6s4d", "AsKdQcJh",
        "AaAh7cTd", "887c2d9h", "AhKh9h2s", "Ah9c5sKs", "98h7s6c"
    };
    
    for (const auto& hand : turn_hands) {
        for (const auto& position : ALL_POSITIONS) {
            for (const auto& board : turn_boards) {
                for (const auto& action : POSTFLOP_ACTIONS) {
                    Scenario scenario;
                    scenario.hand = hand;
                    scenario.position = position;
                    scenario.actions = {"open", "call", "cbet", "call", action};
                    scenario.board = board;
                    scenario.street = Street::TURN;
                    scenario.table_size = table_size;
                    scenario.num_opponents = table_size - 1;
                    
                    scenarios.push_back(scenario);
                }
            }
        }
    }
    
    return scenarios;
}

std::vector<Scenario> PokerAnalyzer::generate_river_scenarios(int table_size) {
    std::vector<Scenario> scenarios;
    
    // Smallest subset for river analysis
    std::vector<std::string> river_hands = {
        "AA", "KK", "AKs", "AKo", "JTs", "98s"
    };
    
    // Sample river boards (flop + turn + river)
    std::vector<std::string> river_boards = {
        "As7h2cKd9s", "9h8d7c6s5h", "Kh9h6s4d2h", "AsKdQcJhTc",
        "AaAh7cTd4s", "887c2d9h3c", "AhKh9h2s8c", "98h7s6c4d"
    };
    
    for (const auto& hand : river_hands) {
        for (const auto& position : ALL_POSITIONS) {
            for (const auto& board : river_boards) {
                for (const auto& action : POSTFLOP_ACTIONS) {
                    Scenario scenario;
                    scenario.hand = hand;
                    scenario.position = position;
                    scenario.actions = {"open", "call", "cbet", "call", "bet", "call", action};
                    scenario.board = board;
                    scenario.street = Street::RIVER;
                    scenario.table_size = table_size;
                    scenario.num_opponents = table_size - 1;
                    
                    scenarios.push_back(scenario);
                }
            }
        }
    }
    
    return scenarios;
}

void PokerAnalyzer::export_results_to_csv(int table_size) {
    std::string output_dir = get_output_directory(table_size);
    
    // Separate results by street
    std::vector<std::pair<Scenario, SimResult>> preflop_results;
    std::vector<std::pair<Scenario, SimResult>> flop_results;
    std::vector<std::pair<Scenario, SimResult>> turn_results;
    std::vector<std::pair<Scenario, SimResult>> river_results;
    
    for (const auto& result_pair : results_) {
        switch (result_pair.first.street) {
            case Street::PREFLOP:
                preflop_results.push_back(result_pair);
                break;
            case Street::FLOP:
                flop_results.push_back(result_pair);
                break;
            case Street::TURN:
                turn_results.push_back(result_pair);
                break;
            case Street::RIVER:
                river_results.push_back(result_pair);
                break;
        }
    }
    
    // Export each street to separate file
    auto export_street = [&](const std::vector<std::pair<Scenario, SimResult>>& street_results, 
                              const std::string& filename) {
        std::ofstream file(output_dir + "/" + filename);
        if (!file.is_open()) {
            std::cerr << "Failed to create file: " << filename << std::endl;
            return;
        }
        
        // Write CSV header
        file << "table_size,hand,position,street,board,action_sequence,optimal_frequency,expected_value,simulations\n";
        
        // Write data
        for (const auto& result_pair : street_results) {
            const Scenario& scenario = result_pair.first;
            const SimResult& result = result_pair.second;
            
            file << std::fixed << std::setprecision(4)
                 << scenario.table_size << ","
                 << scenario.hand << ","
                 << position_to_string(scenario.position) << ","
                 << street_to_string(scenario.street) << ","
                 << scenario.board << ","
                 << scenario.get_action_sequence() << ","
                 << result.optimal_frequency << ","
                 << result.expected_value << ","
                 << result.simulations_run << "\n";
        }
        
        file.close();
        std::cout << "Exported " << street_results.size() << " results to " << filename << std::endl;
    };
    
    export_street(preflop_results, "preflop_analysis.csv");
    export_street(flop_results, "flop_analysis.csv");
    export_street(turn_results, "turn_analysis.csv");
    export_street(river_results, "river_analysis.csv");
}

void PokerAnalyzer::clear_results() {
    results_.clear();
    total_scenarios_ = 0;
    completed_scenarios_ = 0;
}

void PokerAnalyzer::print_progress(const std::string& message) {
    std::cout << "[" << std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count() << "] " << message << std::endl;
}

std::string PokerAnalyzer::get_output_directory(int table_size) {
    return "output/" + std::to_string(table_size) + "max";
}

void PokerAnalyzer::create_output_directories() {
    for (int table_size = 6; table_size <= 9; ++table_size) {
        std::string dir = get_output_directory(table_size);
        std::filesystem::create_directories(dir);
    }
}

} // namespace poker
