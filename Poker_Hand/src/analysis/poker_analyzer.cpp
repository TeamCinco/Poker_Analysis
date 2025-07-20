#include "poker_analyzer.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <chrono>

namespace poker {

PokerAnalyzer::PokerAnalyzer(const Config& config) 
    : config_(config), progress_callback_(nullptr) {
    engine_ = std::make_unique<MonteCarloEngine>(config);
}

PokerAnalyzer::~PokerAnalyzer() = default;

HandAnalysis PokerAnalyzer::analyze_hand(const std::string& hand_str, int simulations_per_opponent) {
    HandAnalysis analysis;
    analysis.hand_string = hand_str;
    analysis.combinations = get_hand_combinations(hand_str);
    
    // Simulate vs 1-9 opponents
    for (int opponents = 1; opponents <= 9; ++opponents) {
        analysis.vs_opponents[opponents - 1] = engine_->simulate_hand(hand_str, opponents, simulations_per_opponent);
    }
    
    // Calculate equity vs different ranges
    auto tight_range = get_tight_range();
    auto loose_range = get_loose_range();
    
    analysis.equity_vs_random = analysis.vs_opponents[0].win_rate; // vs 1 opponent
    analysis.equity_vs_tight = calculate_equity_vs_range(hand_str, tight_range, simulations_per_opponent);
    analysis.equity_vs_loose = calculate_equity_vs_range(hand_str, loose_range, simulations_per_opponent);
    
    // Calculate position-specific actions
    calculate_position_actions(analysis, hand_str);
    
    // GTO analysis
    analyze_gto_frequencies(analysis, hand_str);
    
    return analysis;
}

std::unordered_map<std::string, HandAnalysis> PokerAnalyzer::analyze_all_hands(int simulations_per_hand) {
    auto all_hands = generate_all_starting_hands();
    std::unordered_map<std::string, HandAnalysis> results;
    
    int completed = 0;
    for (const auto& hand : all_hands) {
        if (progress_callback_) {
            progress_callback_(completed, all_hands.size(), hand);
        }
        
        results[hand] = analyze_hand(hand, simulations_per_hand);
        completed++;
    }
    
    return results;
}

HandAnalysis PokerAnalyzer::analyze_hand_position(const std::string& hand_str, Position position, int simulations) {
    HandAnalysis analysis = analyze_hand(hand_str, simulations);
    
    // Focus on specific position
    double ev = calculate_expected_value(hand_str, position, analysis.position_actions[static_cast<int>(position)]);
    analysis.position_ev[static_cast<int>(position)] = ev;
    
    return analysis;
}

std::vector<HandAnalysis> PokerAnalyzer::analyze_range(const std::vector<std::string>& range, int simulations_per_hand) {
    std::vector<HandAnalysis> results;
    results.reserve(range.size());
    
    int completed = 0;
    for (const auto& hand : range) {
        if (progress_callback_) {
            progress_callback_(completed, range.size(), hand);
        }
        
        results.push_back(analyze_hand(hand, simulations_per_hand));
        completed++;
    }
    
    return results;
}

HandAnalysis PokerAnalyzer::analyze_3bet_scenario(const std::string& hand_str, Position hero_pos, Position opener_pos, int simulations) {
    // For now, use basic analysis - in production this would simulate specific 3-bet scenarios
    return analyze_hand_position(hand_str, hero_pos, simulations);
}

HandAnalysis PokerAnalyzer::analyze_4bet_scenario(const std::string& hand_str, Position hero_pos, Position three_bettor_pos, int simulations) {
    // For now, use basic analysis - in production this would simulate specific 4-bet scenarios
    return analyze_hand_position(hand_str, hero_pos, simulations);
}

HandAnalysis PokerAnalyzer::analyze_stack_depth(const std::string& hand_str, int stack_bb, int simulations) {
    HandAnalysis analysis = analyze_hand(hand_str, simulations);
    
    // Adjust for stack depth (simplified)
    double stack_multiplier = 1.0;
    if (stack_bb <= 20) {
        stack_multiplier = 0.8;  // Short stack adjustment
    } else if (stack_bb >= 100) {
        stack_multiplier = 1.2;  // Deep stack adjustment
    }
    
    for (int i = 0; i < 6; ++i) {
        analysis.position_ev[i] *= stack_multiplier;
    }
    
    return analysis;
}

HandAnalysis PokerAnalyzer::quick_analyze(const std::string& hand_str, int max_time_ms) {
    // Use adaptive simulation based on time constraint
    int simulations = std::min(100000, max_time_ms * 1000); // Rough estimate
    return analyze_hand(hand_str, simulations);
}

PerformanceStats PokerAnalyzer::get_performance_stats() const {
    return engine_->get_performance_stats();
}

void PokerAnalyzer::set_config(const Config& config) {
    config_ = config;
    engine_->set_config(config);
}

void PokerAnalyzer::generate_training_data(const std::string& output_directory) {
    log_progress("Starting comprehensive poker analysis data generation...");
    
    // Generate all data files
    export_hand_equity_data(output_directory + "/hand_equity_data.json");
    export_position_ranges(output_directory + "/position_ranges.json");
    export_scenario_analysis(output_directory + "/scenario_analysis.json");
    export_stack_depth_adjustments(output_directory + "/stack_depth_adjustments.json");
    
    log_progress("Training data generation completed!");
}

std::vector<ActionResult> PokerAnalyzer::analyze_position_comprehensive(
    const std::string& position, 
    TableSize table_size, 
    PokerAction action,
    int simulations_per_hand) {
    
    std::vector<ActionResult> results;
    auto all_hands = generate_all_starting_hands();
    
    // Convert action enum to string
    std::string action_name;
    switch (action) {
        case PokerAction::OPEN_RAISE: action_name = "opening_raise"; break;
        case PokerAction::THREE_BET: action_name = "3_bet"; break;
        case PokerAction::FOUR_BET: action_name = "4_bet"; break;
        case PokerAction::CALL: action_name = "call"; break;
        default: action_name = "unknown"; break;
    }
    
    log_progress("Analyzing " + std::to_string(all_hands.size()) + " hands for " + 
                position + " " + action_name + " (" + 
                (table_size == TableSize::SIX_MAX ? "6-max" : "9-max") + ")");
    
    int completed = 0;
    for (const auto& hand : all_hands) {
        if (progress_callback_) {
            progress_callback_(completed, all_hands.size(), hand);
        }
        
        ActionResult result;
        result.hand = hand;
        result.position = position;
        result.action = action_name;
        result.player_count = static_cast<int>(table_size);
        result.simulations_run = simulations_per_hand;
        
        // Check if hand is in realistic range for this position/action
        result.in_range = RealisticRanges::is_hand_in_range(hand, position, table_size, action);
        
        // Calculate appropriate number of opponents based on table size
        int num_opponents = static_cast<int>(table_size) - 1; // minus hero
        
        // Run simulation with realistic opponent ranges
        SimulationResult sim_result = engine_->simulate_hand(hand, num_opponents, simulations_per_hand);
        
        result.win_rate = sim_result.win_rate;
        result.confidence_interval_low = sim_result.confidence_interval_low;
        result.confidence_interval_high = sim_result.confidence_interval_high;
        result.expected_value = sim_result.expected_value;
        
        results.push_back(result);
        completed++;
    }
    
    log_progress("Completed analysis for " + position + " " + action_name);
    return results;
}

void PokerAnalyzer::generate_realistic_analysis_data(
    const std::string& output_directory,
    int simulations_per_hand) {
    
    log_progress("Starting realistic poker range analysis...");
    
    // Create folder structure first
    CSVExporter::create_folder_structure(output_directory);
    
    // Actions to analyze (prioritize opening ranges first)
    std::vector<PokerAction> actions = {
        PokerAction::OPEN_RAISE,
        PokerAction::THREE_BET,
        PokerAction::FOUR_BET
        // PokerAction::CALL can be added later
    };
    
    // Analyze both table sizes
    std::vector<TableSize> table_sizes = {TableSize::SIX_MAX, TableSize::NINE_MAX};
    
    for (TableSize table_size : table_sizes) {
        std::string table_name = (table_size == TableSize::SIX_MAX) ? "6-max" : "9-max";
        auto position_names = RealisticRanges::get_position_names(table_size);
        
        for (const auto& position : position_names) {
            for (PokerAction action : actions) {
                log_progress("Analyzing " + position + " for " + table_name);
                
                auto results = analyze_position_comprehensive(position, table_size, action, simulations_per_hand);
                
                std::string action_name;
                switch (action) {
                    case PokerAction::OPEN_RAISE: action_name = "opening_raise"; break;
                    case PokerAction::THREE_BET: action_name = "3_bet"; break;
                    case PokerAction::FOUR_BET: action_name = "4_bet"; break;
                    default: continue; // Skip unknown actions
                }
                
                CSVExporter::export_position_action_results(results, position, action_name, table_size, output_directory);
            }
        }
    }
    
    log_progress("Realistic analysis data generation completed!");
}

void PokerAnalyzer::export_position_action_csv(
    const std::string& position,
    const std::string& action_name,
    TableSize table_size,
    const std::string& output_directory,
    int simulations_per_hand) {
    
    // Convert action name to enum
    PokerAction action;
    if (action_name == "opening_raise") action = PokerAction::OPEN_RAISE;
    else if (action_name == "3_bet") action = PokerAction::THREE_BET;
    else if (action_name == "4_bet") action = PokerAction::FOUR_BET;
    else if (action_name == "call") action = PokerAction::CALL;
    else {
        log_progress("Unknown action: " + action_name);
        return;
    }
    
    auto results = analyze_position_comprehensive(position, table_size, action, simulations_per_hand);
    CSVExporter::export_position_action_results(results, position, action_name, table_size, output_directory);
}

void PokerAnalyzer::set_progress_callback(ProgressCallback callback) {
    progress_callback_ = callback;
}

std::vector<std::string> PokerAnalyzer::generate_all_starting_hands() const {
    std::vector<std::string> hands;
    hands.reserve(169);
    
    // Generate pairs
    const char* ranks = "23456789TJQKA";
    for (int i = 0; i < 13; ++i) {
        std::string pair = std::string(2, ranks[i]);
        hands.push_back(pair);
    }
    
    // Generate suited and offsuit hands
    for (int i = 12; i >= 0; --i) {  // Start from Ace
        for (int j = i - 1; j >= 0; --j) {
            std::string suited = std::string(1, ranks[i]) + std::string(1, ranks[j]) + "s";
            std::string offsuit = std::string(1, ranks[i]) + std::string(1, ranks[j]) + "o";
            hands.push_back(suited);
            hands.push_back(offsuit);
        }
    }
    
    return hands;
}

int PokerAnalyzer::get_hand_combinations(const std::string& hand_str) const {
    if (hand_str.length() == 2) {
        // Pair
        return 6;
    } else if (hand_str.length() == 3) {
        if (hand_str[2] == 's') {
            // Suited
            return 4;
        } else {
            // Offsuit
            return 12;
        }
    }
    return 0;
}

double PokerAnalyzer::calculate_equity_vs_range(const std::string& hand_str, const std::vector<std::string>& range, int simulations) const {
    // Simplified: simulate vs one opponent with average equity
    // In production, this would specifically simulate against the range
    SimulationResult result = engine_->simulate_hand(hand_str, 1, simulations);
    
    // Adjust based on range tightness
    double adjustment = 1.0;
    if (range.size() < 20) {
        adjustment = 0.8;  // Tighter range
    } else if (range.size() > 50) {
        adjustment = 1.2;  // Looser range
    }
    
    return result.win_rate * adjustment;
}

void PokerAnalyzer::analyze_gto_frequencies(HandAnalysis& analysis, const std::string& hand_str) const {
    // Simplified GTO analysis - in production this would use more sophisticated calculations
    for (int pos = 0; pos < 6; ++pos) {
        Position position = static_cast<Position>(pos);
        auto range = get_position_range(position);
        
        if (std::find(range.begin(), range.end(), hand_str) != range.end()) {
            analysis.position_actions[pos] = Action::RAISE;
            analysis.position_frequencies[pos] = 1.0;
        } else {
            analysis.position_actions[pos] = Action::FOLD;
            analysis.position_frequencies[pos] = 0.0;
        }
    }
}

void PokerAnalyzer::calculate_position_actions(HandAnalysis& analysis, const std::string& hand_str) const {
    for (int pos = 0; pos < 6; ++pos) {
        Position position = static_cast<Position>(pos);
        analysis.position_ev[pos] = calculate_expected_value(hand_str, position, analysis.position_actions[pos]);
    }
}

double PokerAnalyzer::calculate_expected_value(const std::string& hand_str, Position position, Action action) const {
    // Simplified EV calculation
    double base_ev = 0.0;
    
    if (action == Action::RAISE) {
        base_ev = 0.5;  // Positive EV for raising
    } else if (action == Action::CALL) {
        base_ev = 0.0;  // Neutral EV for calling
    } else {
        base_ev = -0.5; // Negative EV for folding
    }
    
    // Position adjustment
    double position_multiplier = 1.0;
    switch (position) {
        case Position::UTG: position_multiplier = 0.8; break;
        case Position::MP: position_multiplier = 0.9; break;
        case Position::CO: position_multiplier = 1.0; break;
        case Position::BTN: position_multiplier = 1.2; break;
        case Position::SB: position_multiplier = 0.7; break;
        case Position::BB: position_multiplier = 0.8; break;
    }
    
    return base_ev * position_multiplier;
}

std::vector<std::string> PokerAnalyzer::get_position_range(Position position) const {
    switch (position) {
        case Position::UTG:
            return {"AA", "KK", "QQ", "JJ", "TT", "99", "88", "AKs", "AKo", "AQs", "AQo", "AJs", "KQs"};
        case Position::MP:
            return {"AA", "KK", "QQ", "JJ", "TT", "99", "88", "77", "AKs", "AKo", "AQs", "AQo", "AJs", "AJo", "ATs", "KQs", "KQo", "KJs"};
        case Position::CO:
            return {"AA", "KK", "QQ", "JJ", "TT", "99", "88", "77", "66", "55", "AKs", "AKo", "AQs", "AQo", "AJs", "AJo", "ATs", "A9s", "KQs", "KQo", "KJs", "KJo", "KTs", "QJs", "QJo", "QTs", "JTs"};
        case Position::BTN:
            return {"AA", "KK", "QQ", "JJ", "TT", "99", "88", "77", "66", "55", "44", "33", "22", "AKs", "AKo", "AQs", "AQo", "AJs", "AJo", "ATs", "A9s", "A8s", "A7s", "A6s", "A5s", "A4s", "A3s", "A2s", "KQs", "KQo", "KJs", "KJo", "KTs", "K9s", "K8s", "K7s", "K6s", "K5s", "K4s", "K3s", "K2s", "QJs", "QJo", "QTs", "Q9s", "Q8s", "Q7s", "Q6s", "Q5s", "Q4s", "Q3s", "Q2s", "JTs", "J9s", "J8s", "J7s", "J6s", "J5s", "J4s", "J3s", "J2s", "T9s", "T8s", "T7s", "T6s", "T5s", "T4s", "T3s", "T2s", "98s", "97s", "96s", "95s", "94s", "93s", "92s", "87s", "86s", "85s", "84s", "83s", "82s", "76s", "75s", "74s", "73s", "72s", "65s", "64s", "63s", "62s", "54s", "53s", "52s", "43s", "42s", "32s"};
        case Position::SB:
            return {"AA", "KK", "QQ", "JJ", "TT", "99", "88", "77", "66", "55", "44", "33", "22", "AKs", "AKo", "AQs", "AQo", "AJs", "AJo", "ATs", "A9s", "A8s", "A7s", "A6s", "A5s", "A4s", "A3s", "A2s", "KQs", "KQo", "KJs", "KJo", "KTs", "K9s", "K8s", "K7s", "K6s", "K5s", "K4s", "K3s", "K2s"};
        case Position::BB:
            return {"AA", "KK", "QQ", "JJ", "TT", "99", "88", "77", "66", "55", "44", "33", "22", "AKs", "AKo", "AQs", "AQo", "AJs", "AJo", "ATs", "A9s", "A8s", "A7s", "A6s", "A5s", "A4s", "A3s", "A2s", "KQs", "KQo", "KJs", "KJo", "KTs", "K9s", "K8s", "K7s", "K6s", "K5s", "K4s", "K3s", "K2s", "QJs", "QJo", "QTs", "Q9s", "Q8s", "Q7s", "Q6s", "Q5s", "Q4s", "Q3s", "Q2s"};
    }
    return {};
}

std::vector<std::string> PokerAnalyzer::get_tight_range() const {
    return {"AA", "KK", "QQ", "JJ", "TT", "99", "88", "AKs", "AKo", "AQs", "AQo", "AJs", "KQs", "KQo", "KJs"};
}

std::vector<std::string> PokerAnalyzer::get_loose_range() const {
    return get_position_range(Position::BTN);  // Use button range as loose range
}

bool PokerAnalyzer::validate_hand_analysis(const HandAnalysis& analysis) const {
    // Basic validation
    if (analysis.equity_vs_random < 0.0 || analysis.equity_vs_random > 1.0) return false;
    if (analysis.equity_vs_tight < 0.0 || analysis.equity_vs_tight > 1.0) return false;
    if (analysis.equity_vs_loose < 0.0 || analysis.equity_vs_loose > 1.0) return false;
    
    for (const auto& result : analysis.vs_opponents) {
        if (result.win_rate < 0.0 || result.win_rate > 1.0) return false;
        if (result.tie_rate < 0.0 || result.tie_rate > 1.0) return false;
        if (result.loss_rate < 0.0 || result.loss_rate > 1.0) return false;
    }
    
    return true;
}

void PokerAnalyzer::export_hand_equity_data(const std::string& output_path) {
    log_progress("Generating hand equity data...");
    
    std::ofstream file(output_path);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << output_path << std::endl;
        return;
    }
    
    file << "{\n";
    file << "  \"hand_equity_data\": {\n";
    
    auto all_hands = generate_all_starting_hands();
    for (size_t i = 0; i < all_hands.size(); ++i) {
        const auto& hand = all_hands[i];
        if (progress_callback_) {
            progress_callback_(i, all_hands.size(), hand);
        }
        
        HandAnalysis analysis = analyze_hand(hand);
        
        file << "    \"" << hand << "\": {\n";
        file << "      \"combinations\": " << analysis.combinations << ",\n";
        file << "      \"equity_vs_random\": " << std::fixed << std::setprecision(4) << analysis.equity_vs_random << ",\n";
        file << "      \"equity_vs_tight\": " << std::fixed << std::setprecision(4) << analysis.equity_vs_tight << ",\n";
        file << "      \"equity_vs_loose\": " << std::fixed << std::setprecision(4) << analysis.equity_vs_loose << ",\n";
        file << "      \"vs_opponents\": {\n";
        
        for (int opp = 0; opp < 9; ++opp) {
            const auto& result = analysis.vs_opponents[opp];
            file << "        \"" << (opp + 1) << "\": {\n";
            file << "          \"win_rate\": " << std::fixed << std::setprecision(4) << result.win_rate << ",\n";
            file << "          \"tie_rate\": " << std::fixed << std::setprecision(4) << result.tie_rate << ",\n";
            file << "          \"loss_rate\": " << std::fixed << std::setprecision(4) << result.loss_rate << ",\n";
            file << "          \"simulations\": " << result.total_simulations << ",\n";
            file << "          \"confidence_interval\": [" << result.confidence_interval_low << ", " << result.confidence_interval_high << "]\n";
            file << "        }";
            if (opp < 8) file << ",";
            file << "\n";
        }
        
        file << "      }\n";
        file << "    }";
        if (i < all_hands.size() - 1) file << ",";
        file << "\n";
    }
    
    file << "  }\n";
    file << "}\n";
    
    file.close();
    log_progress("Hand equity data exported to " + output_path);
}

void PokerAnalyzer::export_position_ranges(const std::string& output_path) const {
    log_progress("Generating position ranges...");
    
    std::ofstream file(output_path);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << output_path << std::endl;
        return;
    }
    
    file << "{\n";
    file << "  \"position_ranges\": {\n";
    
    const char* position_names[] = {"UTG", "MP", "CO", "BTN", "SB", "BB"};
    
    for (int pos = 0; pos < 6; ++pos) {
        Position position = static_cast<Position>(pos);
        auto range = get_position_range(position);
        
        file << "    \"" << position_names[pos] << "\": {\n";
        file << "      \"opening_range\": [";
        
        for (size_t i = 0; i < range.size(); ++i) {
            file << "\"" << range[i] << "\"";
            if (i < range.size() - 1) file << ", ";
        }
        
        file << "],\n";
        file << "      \"range_size\": " << range.size() << ",\n";
        file << "      \"percentage\": " << std::fixed << std::setprecision(1) << (range.size() / 169.0 * 100.0) << "\n";
        file << "    }";
        if (pos < 5) file << ",";
        file << "\n";
    }
    
    file << "  }\n";
    file << "}\n";
    
    file.close();
    log_progress("Position ranges exported to " + output_path);
}

void PokerAnalyzer::export_scenario_analysis(const std::string& output_path) const {
    log_progress("Generating scenario analysis...");
    
    std::ofstream file(output_path);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << output_path << std::endl;
        return;
    }
    
    file << "{\n";
    file << "  \"scenario_analysis\": {\n";
    file << "    \"3bet_scenarios\": {},\n";
    file << "    \"4bet_scenarios\": {},\n";
    file << "    \"squeeze_scenarios\": {}\n";
    file << "  }\n";
    file << "}\n";
    
    file.close();
    log_progress("Scenario analysis exported to " + output_path);
}

void PokerAnalyzer::export_stack_depth_adjustments(const std::string& output_path) const {
    log_progress("Generating stack depth adjustments...");
    
    std::ofstream file(output_path);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << output_path << std::endl;
        return;
    }
    
    file << "{\n";
    file << "  \"stack_depth_adjustments\": {\n";
    file << "    \"short_stack\": {\"bb_range\": \"10-30\", \"multiplier\": 0.8},\n";
    file << "    \"medium_stack\": {\"bb_range\": \"30-100\", \"multiplier\": 1.0},\n";
    file << "    \"deep_stack\": {\"bb_range\": \"100+\", \"multiplier\": 1.2}\n";
    file << "  }\n";
    file << "}\n";
    
    file.close();
    log_progress("Stack depth adjustments exported to " + output_path);
}

void PokerAnalyzer::log_progress(const std::string& message) const {
    std::cout << "[" << std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count() << "] " << message << std::endl;
}

std::string PokerAnalyzer::format_hand_analysis(const HandAnalysis& analysis) const {
    std::ostringstream oss;
    oss << "Hand: " << analysis.hand_string << "\n";
    oss << "Combinations: " << analysis.combinations << "\n";
    oss << "Equity vs Random: " << std::fixed << std::setprecision(2) << (analysis.equity_vs_random * 100) << "%\n";
    oss << "Equity vs Tight: " << std::fixed << std::setprecision(2) << (analysis.equity_vs_tight * 100) << "%\n";
    oss << "Equity vs Loose: " << std::fixed << std::setprecision(2) << (analysis.equity_vs_loose * 100) << "%\n";
    return oss.str();
}

} // namespace poker
