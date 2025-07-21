#include "dynamic_monte_carlo_analyzer.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <iomanip>
#include <chrono>
#include <algorithm>
#include <sstream>
#include <set>
#include <mutex>

namespace poker {

// ===== DiscoveredStrategy Implementation =====

std::string DiscoveredStrategy::get_unique_id() const {
    std::stringstream ss;
    ss << hand << "_" << static_cast<int>(position) << "_" << static_cast<int>(street) 
       << "_" << board << "_" << decision_type << "_" << table_size;
    
    // Add action sequence
    for (const auto& action : actions) {
        ss << "_" << action;
    }
    
    return ss.str();
}

// ===== DynamicHandGenerator Implementation =====

DynamicHandGenerator::DynamicHandGenerator(const AnalysisLimits& limits) : limits_(limits) {}

std::vector<std::string> DynamicHandGenerator::generate_all_hands() const {
    std::vector<std::string> all_hands;
    
    // Generate pairs if "pair" is in suit combinations
    if (std::find(limits_.suit_combinations.begin(), limits_.suit_combinations.end(), "pair") 
        != limits_.suit_combinations.end()) {
        auto pairs = generate_pairs();
        all_hands.insert(all_hands.end(), pairs.begin(), pairs.end());
    }
    
    // Generate suited hands if "suited" is in suit combinations
    if (std::find(limits_.suit_combinations.begin(), limits_.suit_combinations.end(), "suited") 
        != limits_.suit_combinations.end()) {
        auto suited = generate_suited_hands();
        all_hands.insert(all_hands.end(), suited.begin(), suited.end());
    }
    
    // Generate offsuit hands if "offsuit" is in suit combinations
    if (std::find(limits_.suit_combinations.begin(), limits_.suit_combinations.end(), "offsuit") 
        != limits_.suit_combinations.end()) {
        auto offsuit = generate_offsuit_hands();
        all_hands.insert(all_hands.end(), offsuit.begin(), offsuit.end());
    }
    
    return all_hands;
}

std::vector<std::string> DynamicHandGenerator::generate_premium_hands() const {
    std::vector<std::string> premium_hands;
    
    // Premium pairs
    std::vector<std::string> premium_ranks = {"A", "K", "Q", "J", "T", "9"};
    for (const auto& rank : premium_ranks) {
        if (std::find(limits_.rank_range.begin(), limits_.rank_range.end(), rank) != limits_.rank_range.end()) {
            premium_hands.push_back(rank + rank);
        }
    }
    
    // Premium suited hands
    std::vector<std::pair<std::string, std::string>> premium_suited = {
        {"A", "K"}, {"A", "Q"}, {"A", "J"}, {"A", "T"},
        {"K", "Q"}, {"K", "J"}, {"Q", "J"}, {"J", "T"}
    };
    
    for (const auto& hand : premium_suited) {
        if (std::find(limits_.rank_range.begin(), limits_.rank_range.end(), hand.first) != limits_.rank_range.end() &&
            std::find(limits_.rank_range.begin(), limits_.rank_range.end(), hand.second) != limits_.rank_range.end()) {
            premium_hands.push_back(hand.first + hand.second + "s");
        }
    }
    
    return premium_hands;
}

std::vector<std::string> DynamicHandGenerator::generate_speculative_hands() const {
    std::vector<std::string> speculative_hands;
    
    // Suited connectors and one-gappers
    for (size_t i = 0; i < limits_.rank_range.size() - 1; ++i) {
        for (size_t j = i + 1; j < std::min(i + 4, limits_.rank_range.size()); ++j) {
            std::string hand = limits_.rank_range[i] + limits_.rank_range[j] + "s";
            double strength = calculate_hand_strength(hand);
            if (strength >= 0.3 && strength <= 0.7) {  // Speculative range
                speculative_hands.push_back(hand);
            }
        }
    }
    
    return speculative_hands;
}

std::vector<std::string> DynamicHandGenerator::generate_hands_by_strength(double min_strength, double max_strength) const {
    std::vector<std::string> filtered_hands;
    auto all_hands = generate_all_hands();
    
    for (const auto& hand : all_hands) {
        double strength = calculate_hand_strength(hand);
        if (strength >= min_strength && strength <= max_strength) {
            filtered_hands.push_back(hand);
        }
    }
    
    return filtered_hands;
}

std::vector<std::string> DynamicHandGenerator::generate_pairs() const {
    std::vector<std::string> pairs;
    for (const auto& rank : limits_.rank_range) {
        pairs.push_back(rank + rank);
    }
    return pairs;
}

std::vector<std::string> DynamicHandGenerator::generate_suited_hands() const {
    std::vector<std::string> suited_hands;
    
    for (size_t i = 0; i < limits_.rank_range.size(); ++i) {
        for (size_t j = i + 1; j < limits_.rank_range.size(); ++j) {
            suited_hands.push_back(limits_.rank_range[i] + limits_.rank_range[j] + "s");
        }
    }
    
    return suited_hands;
}

std::vector<std::string> DynamicHandGenerator::generate_offsuit_hands() const {
    std::vector<std::string> offsuit_hands;
    
    for (size_t i = 0; i < limits_.rank_range.size(); ++i) {
        for (size_t j = i + 1; j < limits_.rank_range.size(); ++j) {
            offsuit_hands.push_back(limits_.rank_range[i] + limits_.rank_range[j] + "o");
        }
    }
    
    return offsuit_hands;
}

double DynamicHandGenerator::calculate_hand_strength(const std::string& hand) const {
    // Simplified hand strength calculation based on rank values
    std::map<char, double> rank_values = {
        {'A', 14}, {'K', 13}, {'Q', 12}, {'J', 11}, {'T', 10},
        {'9', 9}, {'8', 8}, {'7', 7}, {'6', 6}, {'5', 5}, {'4', 4}, {'3', 3}, {'2', 2}
    };
    
    if (hand.length() == 2) {  // Pair
        double rank_val = rank_values[hand[0]];
        return (rank_val + 10) / 24.0;  // Pairs are stronger
    } else if (hand.length() == 3) {  // Two cards
        double rank1_val = rank_values[hand[0]];
        double rank2_val = rank_values[hand[1]];
        bool suited = (hand[2] == 's');
        
        double base_strength = (rank1_val + rank2_val) / 28.0;
        if (suited) base_strength += 0.1;  // Suited bonus
        
        return std::min(1.0, base_strength);
    }
    
    return 0.5;  // Default
}

// ===== DynamicBoardGenerator Implementation =====

DynamicBoardGenerator::DynamicBoardGenerator(const AnalysisLimits& limits) 
    : limits_(limits), rng_(std::random_device{}()) {}

std::vector<std::string> DynamicBoardGenerator::generate_flop_boards() const {
    std::vector<std::string> boards;
    
    // Generate systematic board combinations up to max_board_samples
    std::vector<std::string> ranks = limits_.rank_range;
    std::vector<char> suits = {'h', 'd', 'c', 's'};
    
    int boards_generated = 0;
    
    // Dry boards
    for (size_t i = 0; i < ranks.size() && boards_generated < limits_.max_board_samples / 3; ++i) {
        for (size_t j = i + 2; j < ranks.size() && boards_generated < limits_.max_board_samples / 3; ++j) {
            for (size_t k = j + 2; k < ranks.size() && boards_generated < limits_.max_board_samples / 3; ++k) {
                std::string board = ranks[i] + "h" + ranks[j] + "d" + ranks[k] + "c";
                boards.push_back(board);
                boards_generated++;
            }
        }
    }
    
    // Wet boards (connected)
    std::uniform_int_distribution<> rank_dist(0, ranks.size() - 3);
    std::uniform_int_distribution<> suit_dist(0, 3);
    
    while (boards_generated < (2 * limits_.max_board_samples) / 3) {
        int start_idx = rank_dist(rng_);
        std::string board = ranks[start_idx] + suits[suit_dist(rng_)] + 
                           ranks[start_idx + 1] + suits[suit_dist(rng_)] + 
                           ranks[start_idx + 2] + suits[suit_dist(rng_)];
        boards.push_back(board);
        boards_generated++;
    }
    
    // Paired boards
    while (boards_generated < limits_.max_board_samples) {
        int pair_idx = rank_dist(rng_);
        int kicker_idx = (pair_idx + 2) % ranks.size();
        std::string board = ranks[pair_idx] + "h" + ranks[pair_idx] + "d" + ranks[kicker_idx] + "c";
        boards.push_back(board);
        boards_generated++;
    }
    
    return boards;
}

std::vector<std::string> DynamicBoardGenerator::generate_turn_boards() const {
    auto flop_boards = generate_flop_boards();
    std::vector<std::string> turn_boards;
    
    std::vector<std::string> ranks = limits_.rank_range;
    std::vector<char> suits = {'h', 'd', 'c', 's'};
    std::uniform_int_distribution<> rank_dist(0, ranks.size() - 1);
    std::uniform_int_distribution<> suit_dist(0, 3);
    
    // Add turn card to subset of flop boards
    for (size_t i = 0; i < std::min(static_cast<size_t>(limits_.max_board_samples / 2), flop_boards.size()); ++i) {
        std::string turn_card = ranks[rank_dist(rng_)] + std::string(1, suits[suit_dist(rng_)]);
        turn_boards.push_back(flop_boards[i] + turn_card);
    }
    
    return turn_boards;
}

std::vector<std::string> DynamicBoardGenerator::generate_river_boards() const {
    auto turn_boards = generate_turn_boards();
    std::vector<std::string> river_boards;
    
    std::vector<std::string> ranks = limits_.rank_range;
    std::vector<char> suits = {'h', 'd', 'c', 's'};
    std::uniform_int_distribution<> rank_dist(0, ranks.size() - 1);
    std::uniform_int_distribution<> suit_dist(0, 3);
    
    // Add river card to subset of turn boards
    for (size_t i = 0; i < std::min(static_cast<size_t>(limits_.max_board_samples / 3), turn_boards.size()); ++i) {
        std::string river_card = ranks[rank_dist(rng_)] + std::string(1, suits[suit_dist(rng_)]);
        river_boards.push_back(turn_boards[i] + river_card);
    }
    
    return river_boards;
}

bool DynamicBoardGenerator::is_dry_board(const std::string& board) const {
    // Simplified: check if no consecutive ranks
    if (board.length() < 6) return true;
    
    std::vector<char> ranks = {board[0], board[2], board[4]};
    std::sort(ranks.begin(), ranks.end());
    
    // Check for no straights or flush draws
    bool has_consecutive = false;
    for (size_t i = 0; i < ranks.size() - 1; ++i) {
        if (abs(ranks[i] - ranks[i + 1]) == 1) {
            has_consecutive = true;
            break;
        }
    }
    
    return !has_consecutive;
}

bool DynamicBoardGenerator::is_wet_board(const std::string& board) const {
    return !is_dry_board(board);
}

bool DynamicBoardGenerator::is_paired_board(const std::string& board) const {
    if (board.length() < 6) return false;
    return (board[0] == board[2] || board[0] == board[4] || board[2] == board[4]);
}

// ===== ActionTreeExplorer Implementation =====

ActionTreeExplorer::ActionTreeExplorer(const AnalysisLimits& limits, MonteCarloEngine& engine)
    : limits_(limits), engine_(engine), board_generator_(limits) {}

std::vector<DiscoveredStrategy> ActionTreeExplorer::explore_hand_position(
    const std::string& hand, Position position, int table_size) {
    
    std::vector<DiscoveredStrategy> discovered_strategies;
    
    // Start exploration with empty action sequence on preflop
    explore_action_tree(hand, position, {}, "", Street::PREFLOP, table_size, discovered_strategies);
    
    return discovered_strategies;
}

void ActionTreeExplorer::explore_action_tree(
    const std::string& hand,
    Position position,
    std::vector<std::string> current_sequence,
    const std::string& board,
    Street street,
    int table_size,
    std::vector<DiscoveredStrategy>& discovered_strategies) {
    
    // Stop if we've reached max depth
    if (current_sequence.size() >= static_cast<size_t>(limits_.max_action_depth)) return;
    
    // Test each possible next action
    for (const auto& action : limits_.actions) {
        // Check if this action is valid in current context
        if (!is_valid_action(action, street, current_sequence)) continue;
        
        // Test all frequencies for this action
        DiscoveredStrategy best_result = test_action_frequencies(
            hand, position, current_sequence, board, street, action, table_size
        );
        
        // If this action is profitable, save it and continue exploring
        if (best_result.expected_value >= limits_.min_profitable_ev) {
            discovered_strategies.push_back(best_result);
            
            // Continue exploring this profitable path
            auto new_sequence = current_sequence;
            new_sequence.push_back(action);
            
            // Get next state
            auto next_state = get_next_state(street, board, action);
            Street next_street = next_state.first;
            std::string next_board = next_state.second;
            
            // If we moved to a postflop street and need a board
            if (next_street != Street::PREFLOP && next_board.empty()) {
                // Generate representative boards for this street
                std::vector<std::string> boards;
                switch (next_street) {
                    case Street::FLOP:
                        boards = board_generator_.generate_flop_boards();
                        break;
                    case Street::TURN:
                        boards = board_generator_.generate_turn_boards();
                        break;
                    case Street::RIVER:
                        boards = board_generator_.generate_river_boards();
                        break;
                    default:
                        continue;
                }
                
                // Explore a sample of boards (to keep computation manageable)
                size_t max_boards = std::min(static_cast<size_t>(5), boards.size());
                for (size_t i = 0; i < max_boards; ++i) {
                    explore_action_tree(hand, position, new_sequence, boards[i], 
                                      next_street, table_size, discovered_strategies);
                }
            } else {
                // Continue with current board
                explore_action_tree(hand, position, new_sequence, next_board, 
                                  next_street, table_size, discovered_strategies);
            }
        }
    }
}

bool ActionTreeExplorer::is_valid_action(
    const std::string& action, 
    Street street, 
    const std::vector<std::string>& current_sequence) const {
    
    // Basic validity checks
    if (street == Street::PREFLOP) {
        // Preflop actions
        if (current_sequence.empty()) {
            return (action == "open" || action == "call" || action == "fold");
        } else if (current_sequence.back() == "open") {
            return (action == "3bet" || action == "call" || action == "fold");
        } else if (current_sequence.back() == "3bet") {
            return (action == "4bet" || action == "call" || action == "fold");
        } else if (current_sequence.back() == "4bet") {
            return (action == "5bet" || action == "call" || action == "fold");
        }
    } else {
        // Postflop actions
        if (action == "open" || action == "3bet" || action == "4bet" || action == "5bet") {
            return false;  // These are preflop-only actions
        }
        
        // First action on a street can be bet, check, or (if facing action) call/raise/fold
        if (current_sequence.empty() || 
            std::find(current_sequence.begin(), current_sequence.end(), "check") != current_sequence.end()) {
            return (action == "bet" || action == "check" || action == "cbet");
        }
        
        return (action == "call" || action == "raise" || action == "fold");
    }
    
    return false;
}

DiscoveredStrategy ActionTreeExplorer::test_action_frequencies(
    const std::string& hand,
    Position position,
    const std::vector<std::string>& action_sequence,
    const std::string& board,
    Street street,
    const std::string& action,
    int table_size) {
    
    DiscoveredStrategy result;
    result.hand = hand;
    result.position = position;
    result.actions = action_sequence;
    result.board = board;
    result.street = street;
    result.decision_type = action;
    result.table_size = table_size;
    result.optimal_frequency = 0.0;
    result.expected_value = -999.0;
    result.simulations_run = 0;
    
    // Create scenario for Monte Carlo testing
    Scenario scenario;
    scenario.hand = hand;
    scenario.position = position;
    scenario.actions = action_sequence;
    scenario.actions.push_back(action);  // Add the action being tested
    scenario.board = board;
    scenario.street = street;
    scenario.table_size = table_size;
    scenario.num_opponents = table_size - 1;
    
    // Test each frequency and find the one with highest EV
    double best_ev = -999.0;
    double best_frequency = 0.0;
    int total_simulations = 0;
    
    for (double frequency : limits_.test_frequencies) {
        double ev = engine_.test_frequency(scenario, frequency, limits_.simulations_per_test);
        total_simulations += limits_.simulations_per_test;
        
        if (ev > best_ev) {
            best_ev = ev;
            best_frequency = frequency;
        }
    }
    
    result.optimal_frequency = best_frequency;
    result.expected_value = best_ev;
    result.simulations_run = total_simulations;
    result.confidence_interval = 0.02; // Simplified
    
    return result;
}

std::pair<Street, std::string> ActionTreeExplorer::get_next_state(
    Street current_street,
    const std::string& current_board,
    const std::string& action) const {
    
    // Actions that advance to next street
    std::set<std::string> advancing_actions = {"call", "check"};
    
    if (advancing_actions.find(action) != advancing_actions.end()) {
        switch (current_street) {
            case Street::PREFLOP:
                return {Street::FLOP, ""};  // Board will be generated
            case Street::FLOP:
                return {Street::TURN, current_board};  // Board will be extended
            case Street::TURN:
                return {Street::RIVER, current_board};  // Board will be extended
            case Street::RIVER:
                return {Street::RIVER, current_board};  // Stay on river
        }
    }
    
    // Non-advancing actions stay on same street
    return {current_street, current_board};
}

// ===== DynamicMonteCarloAnalyzer Implementation =====

DynamicMonteCarloAnalyzer::DynamicMonteCarloAnalyzer(const AnalysisLimits& limits)
    : limits_(limits), hand_generator_(limits), action_explorer_(limits, engine_),
      total_combinations_(0), completed_combinations_(0) {
    setup_thread_pool();
}

void DynamicMonteCarloAnalyzer::analyze_within_limits() {
    create_output_directories();
    
    for (int table_size : limits_.table_sizes) {
        print_progress("Starting dynamic analysis for " + std::to_string(table_size) + "-max");
        analyze_table_size(table_size);
    }
    
    print_progress("Dynamic Monte Carlo analysis complete!");
}

void DynamicMonteCarloAnalyzer::analyze_table_size(int table_size) {
    clear_strategies_for_table(table_size);
    
    // Generate all hand combinations within limits
    auto all_hands = hand_generator_.generate_all_hands();
    
    // Calculate total combinations for progress tracking
    total_combinations_ = static_cast<int>(all_hands.size() * limits_.positions.size());
    completed_combinations_ = 0;
    
    print_progress("Testing " + std::to_string(total_combinations_) + " hand/position combinations");
    
    // Submit all combinations to thread pool for parallel processing
    std::vector<std::future<void>> futures;
    
    for (const auto& hand : all_hands) {
        for (const auto& position : limits_.positions) {
            auto future = thread_pool_->enqueue([this, hand, position, table_size]() {
                analyze_hand_position_combination(hand, position, table_size);
            });
            futures.push_back(std::move(future));
        }
    }
    
    // Wait for all tasks to complete
    for (auto& future : futures) {
        future.wait();
    }
    
    // Post-process results
    filter_profitable_strategies(table_size);
    remove_duplicate_strategies(table_size);
    sort_strategies_by_ev(table_size);
    
    // Export results
    export_strategies_to_csv(table_size);
    
    print_progress("Discovered " + std::to_string(discovered_strategies_[table_size].size()) + 
                  " profitable strategies for " + std::to_string(table_size) + "-max");
}

void DynamicMonteCarloAnalyzer::analyze_hand_position_combination(
    const std::string& hand, Position position, int table_size) {
    
    // Use action tree explorer to find all profitable strategies for this combination
    auto strategies = action_explorer_.explore_hand_position(hand, position, table_size);
    
    // Thread-safe insertion of discovered strategies
    {
        static std::mutex strategies_mutex;
        std::lock_guard<std::mutex> lock(strategies_mutex);
        
        for (const auto& strategy : strategies) {
            discovered_strategies_[table_size].push_back(strategy);
        }
    }
    
    // Update progress
    completed_combinations_++;
    
    if (completed_combinations_ % 20 == 0) {
        int current = completed_combinations_;
        int total = total_combinations_;
        double percentage = (static_cast<double>(current) / total) * 100.0;
        print_progress("Progress: " + std::to_string(current) + "/" + std::to_string(total) + 
                      " (" + std::to_string(static_cast<int>(percentage)) + "%)");
    }
}

void DynamicMonteCarloAnalyzer::export_strategies_to_csv(int table_size) const {
    std::string output_dir = get_output_directory(table_size);
    
    auto it = discovered_strategies_.find(table_size);
    if (it == discovered_strategies_.end()) return;
    
    const auto& strategies = it->second;
    
    // Separate strategies by street
    std::map<Street, std::vector<DiscoveredStrategy>> street_strategies;
    for (const auto& strategy : strategies) {
        street_strategies[strategy.street].push_back(strategy);
    }
    
    // Export each street to separate file
    for (const auto& street_pair : street_strategies) {
        Street street = street_pair.first;
        const auto& street_strategies_list = street_pair.second;
        
        std::string filename = output_dir + "/dynamic_" + street_to_string(street) + "_strategy.csv";
        std::ofstream file(filename);
        
        if (!file.is_open()) {
            std::cerr << "Failed to create file: " << filename << std::endl;
            continue;
        }
        
        // Write CSV header
        file << "table_size,hand,position,street,board,decision_type,action_sequence,"
             << "optimal_frequency,expected_value,simulations_run,is_profitable\n";
        
        // Write data
        for (const auto& strategy : street_strategies_list) {
            std::stringstream action_seq;
            for (size_t i = 0; i < strategy.actions.size(); ++i) {
                if (i > 0) action_seq << "_";
                action_seq << strategy.actions[i];
            }
            
            file << std::fixed << std::setprecision(4)
                 << strategy.table_size << ","
                 << strategy.hand << ","
                 << position_to_string(strategy.position) << ","
                 << street_to_string(strategy.street) << ","
                 << strategy.board << ","
                 << strategy.decision_type << ","
                 << action_seq.str() << ","
                 << strategy.optimal_frequency << ","
                 << strategy.expected_value << ","
                 << strategy.simulations_run << ","
                 << (strategy.expected_value >= limits_.min_profitable_ev ? "TRUE" : "FALSE") << "\n";
        }
        
        file.close();
        print_progress("Exported " + std::to_string(street_strategies_list.size()) + " strategies to " + filename);
    }
    
    // Export consolidated profitable strategies only
    std::string profitable_filename = output_dir + "/profitable_strategies_only.csv";
    std::ofstream profitable_file(profitable_filename);
    
    if (profitable_file.is_open()) {
        profitable_file << "table_size,hand,position,street,board,decision_type,action_sequence,"
                       << "optimal_frequency,expected_value,simulations_run\n";
        
        for (const auto& strategy : strategies) {
            if (strategy.expected_value >= limits_.min_profitable_ev) {
                std::stringstream action_seq;
                for (size_t i = 0; i < strategy.actions.size(); ++i) {
                    if (i > 0) action_seq << "_";
                    action_seq << strategy.actions[i];
                }
                
                profitable_file << std::fixed << std::setprecision(4)
                               << strategy.table_size << ","
                               << strategy.hand << ","
                               << position_to_string(strategy.position) << ","
                               << street_to_string(strategy.street) << ","
                               << strategy.board << ","
                               << strategy.decision_type << ","
                               << action_seq.str() << ","
                               << strategy.optimal_frequency << ","
                               << strategy.expected_value << ","
                               << strategy.simulations_run << "\n";
            }
        }
        
        profitable_file.close();
    }
}

const std::vector<DiscoveredStrategy>& DynamicMonteCarloAnalyzer::get_strategies(int table_size) const {
    static const std::vector<DiscoveredStrategy> empty_vector;
    auto it = discovered_strategies_.find(table_size);
    return (it != discovered_strategies_.end()) ? it->second : empty_vector;
}

void DynamicMonteCarloAnalyzer::setup_thread_pool() {
    int num_threads = (limits_.max_threads > 0) ? limits_.max_threads : std::thread::hardware_concurrency();
    thread_pool_ = std::make_unique<ThreadPool>(num_threads);
}

void DynamicMonteCarloAnalyzer::clear_strategies_for_table(int table_size) {
    discovered_strategies_[table_size].clear();
}

void DynamicMonteCarloAnalyzer::print_progress(const std::string& message) const {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    std::cout << "[" << std::put_time(std::localtime(&time_t), "%H:%M:%S") << "] " << message << std::endl;
}

std::string DynamicMonteCarloAnalyzer::get_output_directory(int table_size) const {
    return "output/dynamic_" + std::to_string(table_size) + "max";
}

void DynamicMonteCarloAnalyzer::create_output_directories() const {
    for (int table_size : limits_.table_sizes) {
        std::string dir = get_output_directory(table_size);
        std::filesystem::create_directories(dir);
    }
}

void DynamicMonteCarloAnalyzer::filter_profitable_strategies(int table_size) {
    auto& strategies = discovered_strategies_[table_size];
    
    // Remove strategies that don't meet minimum EV threshold
    strategies.erase(
        std::remove_if(strategies.begin(), strategies.end(),
            [this](const DiscoveredStrategy& strategy) {
                return strategy.expected_value < limits_.min_profitable_ev;
            }),
        strategies.end()
    );
}

void DynamicMonteCarloAnalyzer::remove_duplicate_strategies(int table_size) {
    auto& strategies = discovered_strategies_[table_size];
    
    // Sort by unique ID first
    std::sort(strategies.begin(), strategies.end(),
        [](const DiscoveredStrategy& a, const DiscoveredStrategy& b) {
            return a.get_unique_id() < b.get_unique_id();
        });
    
    // Remove duplicates (keep the one with higher EV)
    auto it = strategies.begin();
    while (it != strategies.end()) {
        auto next_it = it + 1;
        
        // Find all strategies with same unique ID
        while (next_it != strategies.end() && it->get_unique_id() == next_it->get_unique_id()) {
            // Keep the one with higher expected value
            if (next_it->expected_value > it->expected_value) {
                it = strategies.erase(it);
                it = next_it - 1;  // Adjust iterator after erase
            } else {
                next_it = strategies.erase(next_it);
            }
        }
        
        ++it;
    }
}

void DynamicMonteCarloAnalyzer::sort_strategies_by_ev(int table_size) {
    auto& strategies = discovered_strategies_[table_size];
    
    // Sort by expected value (highest first)
    std::sort(strategies.begin(), strategies.end(),
        [](const DiscoveredStrategy& a, const DiscoveredStrategy& b) {
            return a.expected_value > b.expected_value;
        });
}

} // namespace poker
