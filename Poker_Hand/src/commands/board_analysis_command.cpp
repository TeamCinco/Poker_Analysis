#include "board_analysis_command.hpp"
#include "../ui/progress_display.hpp"
#include "../utils/csv_exporter.hpp"
#include <iostream>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <unordered_map>
#include <iomanip>

namespace poker {

BoardAnalysisCommand::BoardAnalysisCommand() = default;

void BoardAnalysisCommand::execute() {
    std::cout << "=== Board Texture Classification Analysis ===" << std::endl;
    std::cout << "This will analyze and classify poker board textures" << std::endl;
    std::cout << "to establish the foundation for postflop analysis." << std::endl << std::endl;
    
    print_help();
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    try {
        std::cout << "Starting board texture analysis..." << std::endl;
        std::cout << "Output directory: ./output/board_analysis/" << std::endl << std::endl;
        
        create_output_directories();
        
        // Run comprehensive board analysis
        std::cout << "Phase 1/4: Analyzing dry boards..." << std::endl;
        test_dry_boards();
        
        std::cout << "\nPhase 2/4: Analyzing wet boards..." << std::endl;
        test_wet_boards();
        
        std::cout << "\nPhase 3/4: Analyzing paired boards..." << std::endl;
        test_paired_boards();
        
        std::cout << "\nPhase 4/4: Analyzing special cases..." << std::endl;
        test_special_cases();
        
        std::cout << "\nGenerating comprehensive analysis CSV..." << std::endl;
        analyze_representative_boards();
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);
        
        std::cout << std::endl << "=== Board Analysis Complete ===" << std::endl;
        std::cout << "Total execution time: " << duration.count() << " seconds" << std::endl;
        std::cout << "Board classifications generated in ./output/board_analysis/" << std::endl;
        std::cout << "  - dry_boards_analysis.csv" << std::endl;
        std::cout << "  - wet_boards_analysis.csv" << std::endl;
        std::cout << "  - paired_boards_analysis.csv" << std::endl;
        std::cout << "  - comprehensive_board_analysis.csv" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error during board analysis: " << e.what() << std::endl;
        throw;
    }
}

std::string BoardAnalysisCommand::get_name() const {
    return "boards";
}

std::string BoardAnalysisCommand::get_description() const {
    return "Analyze and classify poker board textures for strategic analysis";
}

void BoardAnalysisCommand::print_help() const {
    std::cout << "This analysis will classify boards by:" << std::endl;
    std::cout << "• Texture type (dry, wet, paired, monotone, two-tone)" << std::endl;
    std::cout << "• Connectivity level (straight draw potential)" << std::endl;
    std::cout << "• Flush potential (draw strength)" << std::endl;
    std::cout << "• High card bias (range advantage)" << std::endl;
    std::cout << "• Strategic implications (c-bet frequencies, etc.)" << std::endl << std::endl;
    
    std::cout << "Board categories:" << std::endl;
    std::cout << "• Dry: A72r, K83r (disconnected, rainbow)" << std::endl;
    std::cout << "• Wet: 987ss, JT9cc (connected with draws)" << std::endl;
    std::cout << "• Paired: AAx, KKx (trips potential)" << std::endl;
    std::cout << "• Monotone: As7s2s (completed flush)" << std::endl;
    std::cout << "• Special cases: broadway, wheel textures" << std::endl << std::endl;
    
    std::cout << "Expected time: 1-2 minutes" << std::endl << std::endl;
}

void BoardAnalysisCommand::show_progress(int completed, int total, const std::string& current_item) {
    if (total == 0) return;
    
    static int last_percent = -1;
    int percent = (completed * 100) / total;
    
    if (percent != last_percent) {
        std::cout << "\rProgress: [";
        int bar_length = 30;
        int filled = (completed * bar_length) / total;
        
        for (int i = 0; i < bar_length; ++i) {
            if (i < filled) {
                std::cout << "=";
            } else if (i == filled && completed < total) {
                std::cout << ">";
            } else {
                std::cout << " ";
            }
        }
        
        std::cout << "] " << percent << "% " << current_item;
        std::cout.flush();
        
        last_percent = percent;
        
        if (completed == total) {
            std::cout << std::endl;
        }
    }
}

// Board texture classifier implementation
BoardAnalysisCommand::BoardTextureClassifier::BoardTextureClassifier() {
    initialize_strategic_tables();
}

BoardAnalysis BoardAnalysisCommand::BoardTextureClassifier::classify_board_string(const std::string& board_str) {
    // Parse board string (e.g., "As7h2c") into Card array
    std::array<Card, 3> board;
    
    if (board_str.length() != 6) {
        throw std::invalid_argument("Invalid board string format");
    }
    
    for (int i = 0; i < 3; ++i) {
        char rank_char = board_str[i * 2];
        char suit_char = board_str[i * 2 + 1];
        
        int rank = 0;
        switch (rank_char) {
            case '2': rank = 2; break;
            case '3': rank = 3; break;
            case '4': rank = 4; break;
            case '5': rank = 5; break;
            case '6': rank = 6; break;
            case '7': rank = 7; break;
            case '8': rank = 8; break;
            case '9': rank = 9; break;
            case 'T': rank = 10; break;
            case 'J': rank = 11; break;
            case 'Q': rank = 12; break;
            case 'K': rank = 13; break;
            case 'A': rank = 14; break;
            default: throw std::invalid_argument("Invalid rank character");
        }
        
        int suit = 0;
        switch (suit_char) {
            case 'c': suit = 0; break;
            case 'd': suit = 1; break;
            case 'h': suit = 2; break;
            case 's': suit = 3; break;
            default: throw std::invalid_argument("Invalid suit character");
        }
        
        board[i] = make_card(rank, suit);
    }
    
    return classify_board(board);
}

BoardAnalysis BoardAnalysisCommand::BoardTextureClassifier::classify_board(const std::array<Card, 3>& board) {
    BoardAnalysis analysis;
    analysis.board_string = card_to_string(board[0]) + card_to_string(board[1]) + card_to_string(board[2]);
    
    // Calculate core metrics
    analysis.connectivity_index = calculate_connectivity_index(board);
    analysis.flush_potential = calculate_flush_potential(board);
    analysis.pair_potential = calculate_pair_potential(board);
    analysis.high_card_bias = calculate_high_card_bias(board);
    
    // Determine texture classifications
    analysis.primary_texture = determine_primary_texture(board);
    analysis.secondary_texture = determine_secondary_texture(board, analysis.primary_texture);
    
    // Calculate strategic implications
    analysis.expected_cbet_freq = calculate_expected_cbet_freq(analysis.primary_texture);
    analysis.expected_checkraise_freq = calculate_expected_checkraise_freq(analysis.primary_texture);
    analysis.range_advantage_pfr = calculate_range_advantage_pfr(analysis.primary_texture, analysis.connectivity_index);
    
    return analysis;
}

double BoardAnalysisCommand::BoardTextureClassifier::calculate_connectivity_index(const std::array<Card, 3>& board) {
    std::array<int, 3> ranks;
    for (int i = 0; i < 3; ++i) {
        ranks[i] = get_rank(board[i]);
    }
    std::sort(ranks.begin(), ranks.end());
    
    double connectivity = 0.0;
    
    // Check for straight possibilities
    int gap1 = ranks[1] - ranks[0];
    int gap2 = ranks[2] - ranks[1];
    
    // Perfect connectivity (876, JT9)
    if (gap1 == 1 && gap2 == 1) {
        connectivity = 1.0;
    }
    // One gap (975, JT8)
    else if ((gap1 == 1 && gap2 == 2) || (gap1 == 2 && gap2 == 1)) {
        connectivity = 0.7;
    }
    // Two gaps but still connected (964, JT7)
    else if (gap1 <= 2 && gap2 <= 2) {
        connectivity = 0.4;
    }
    // Special case: wheel draws (A32, A43)
    else if (ranks[2] == 14 && ranks[0] <= 5) {
        connectivity = 0.6;
    }
    // Disconnected
    else {
        connectivity = 0.1;
    }
    
    return connectivity;
}

double BoardAnalysisCommand::BoardTextureClassifier::calculate_flush_potential(const std::array<Card, 3>& board) {
    std::unordered_map<int, int> suit_counts;
    for (const Card& card : board) {
        suit_counts[get_suit(card)]++;
    }
    
    // Monotone (all same suit)
    if (suit_counts.size() == 1) {
        return 1.0;
    }
    // Two-tone (flush draw present)
    else if (suit_counts.size() == 2) {
        for (const auto& [suit, count] : suit_counts) {
            if (count == 2) {
                return 0.6; // Two cards of same suit
            }
        }
    }
    // Rainbow (no flush draws)
    return 0.0;
}

double BoardAnalysisCommand::BoardTextureClassifier::calculate_pair_potential(const std::array<Card, 3>& board) {
    std::unordered_map<int, int> rank_counts;
    for (const Card& card : board) {
        rank_counts[get_rank(card)]++;
    }
    
    // Paired board
    for (const auto& [rank, count] : rank_counts) {
        if (count == 2) {
            return 1.0;
        }
    }
    
    // Unpaired but close ranks (potential for two pair)
    std::vector<int> ranks;
    for (const auto& [rank, count] : rank_counts) {
        ranks.push_back(rank);
    }
    std::sort(ranks.begin(), ranks.end());
    
    // Close ranks increase pair potential slightly
    double pair_potential = 0.0;
    for (int i = 1; i < ranks.size(); ++i) {
        if (ranks[i] - ranks[i-1] <= 3) {
            pair_potential += 0.1;
        }
    }
    
    return std::min(pair_potential, 0.3);
}

double BoardAnalysisCommand::BoardTextureClassifier::calculate_high_card_bias(const std::array<Card, 3>& board) {
    double high_card_score = 0.0;
    
    for (const Card& card : board) {
        int rank = get_rank(card);
        if (rank >= 11) { // J, Q, K, A
            high_card_score += 0.4;
        } else if (rank >= 9) { // 9, T
            high_card_score += 0.2;
        } else if (rank >= 7) { // 7, 8
            high_card_score += 0.1;
        }
    }
    
    return std::min(high_card_score, 1.0);
}

BoardTexture BoardAnalysisCommand::BoardTextureClassifier::determine_primary_texture(const std::array<Card, 3>& board) {
    if (is_paired_board(board)) {
        return BoardTexture::PAIRED;
    }
    
    if (is_monotone_board(board)) {
        return BoardTexture::MONOTONE;
    }
    
    if (is_highly_connected(board)) {
        if (is_two_tone_board(board)) {
            return BoardTexture::WET_CONNECTED;
        } else {
            return BoardTexture::HIGHLY_CONNECTED;
        }
    }
    
    double connectivity = calculate_connectivity_index(board);
    if (connectivity >= 0.6) {
        return BoardTexture::MODERATELY_CONNECTED;
    }
    
    if (is_two_tone_board(board)) {
        return BoardTexture::TWO_TONE;
    }
    
    // Check for special dry cases
    std::vector<int> ranks;
    for (const Card& card : board) {
        ranks.push_back(get_rank(card));
    }
    std::sort(ranks.rbegin(), ranks.rend()); // Sort descending
    
    if (ranks[0] == 14 && connectivity < 0.3) { // Ace high dry
        return BoardTexture::ACE_HIGH_DRY;
    }
    
    if (is_broadway_heavy(board)) {
        return BoardTexture::BROADWAY_HEAVY;
    }
    
    if (is_wheel_texture(board)) {
        return BoardTexture::WHEEL_TEXTURE;
    }
    
    return BoardTexture::DRY_RAINBOW;
}

BoardTexture BoardAnalysisCommand::BoardTextureClassifier::determine_secondary_texture(const std::array<Card, 3>& board, BoardTexture primary) {
    // Determine secondary characteristics
    if (primary != BoardTexture::HIGH_BOARD && calculate_high_card_bias(board) > 0.6) {
        return BoardTexture::HIGH_BOARD;
    }
    
    if (primary != BoardTexture::MIDDLE_BOARD) {
        std::vector<int> ranks;
        for (const Card& card : board) {
            ranks.push_back(get_rank(card));
        }
        bool is_middle = std::any_of(ranks.begin(), ranks.end(), [](int rank) { return rank >= 7 && rank <= 10; });
        if (is_middle) {
            return BoardTexture::MIDDLE_BOARD;
        }
    }
    
    if (primary != BoardTexture::LOW_BOARD) {
        std::vector<int> ranks;
        for (const Card& card : board) {
            ranks.push_back(get_rank(card));
        }
        bool is_low = std::all_of(ranks.begin(), ranks.end(), [](int rank) { return rank <= 8; });
        if (is_low) {
            return BoardTexture::LOW_BOARD;
        }
    }
    
    return primary; // No significant secondary texture
}

void BoardAnalysisCommand::BoardTextureClassifier::initialize_strategic_tables() {
    // C-bet frequency tables based on GTO solver data
    cbet_frequency_table_[BoardTexture::DRY_RAINBOW] = 0.75;
    cbet_frequency_table_[BoardTexture::WET_CONNECTED] = 0.45;
    cbet_frequency_table_[BoardTexture::PAIRED] = 0.65;
    cbet_frequency_table_[BoardTexture::MONOTONE] = 0.30;
    cbet_frequency_table_[BoardTexture::TWO_TONE] = 0.55;
    cbet_frequency_table_[BoardTexture::ACE_HIGH_DRY] = 0.85;
    cbet_frequency_table_[BoardTexture::BROADWAY_HEAVY] = 0.70;
    cbet_frequency_table_[BoardTexture::WHEEL_TEXTURE] = 0.40;
    
    // Check-raise frequency tables
    checkraise_frequency_table_[BoardTexture::DRY_RAINBOW] = 0.12;
    checkraise_frequency_table_[BoardTexture::WET_CONNECTED] = 0.25;
    checkraise_frequency_table_[BoardTexture::PAIRED] = 0.18;
    checkraise_frequency_table_[BoardTexture::MONOTONE] = 0.35;
    checkraise_frequency_table_[BoardTexture::TWO_TONE] = 0.20;
    
    // Range advantage for PFR
    range_advantage_table_[BoardTexture::DRY_RAINBOW] = 0.75;
    range_advantage_table_[BoardTexture::WET_CONNECTED] = 0.35;
    range_advantage_table_[BoardTexture::PAIRED] = 0.55;
    range_advantage_table_[BoardTexture::ACE_HIGH_DRY] = 0.85;
    range_advantage_table_[BoardTexture::BROADWAY_HEAVY] = 0.70;
}

// Helper functions for board classification
bool BoardAnalysisCommand::BoardTextureClassifier::is_paired_board(const std::array<Card, 3>& board) {
    return get_rank(board[0]) == get_rank(board[1]) ||
           get_rank(board[0]) == get_rank(board[2]) ||
           get_rank(board[1]) == get_rank(board[2]);
}

bool BoardAnalysisCommand::BoardTextureClassifier::is_monotone_board(const std::array<Card, 3>& board) {
    return get_suit(board[0]) == get_suit(board[1]) && 
           get_suit(board[1]) == get_suit(board[2]);
}

bool BoardAnalysisCommand::BoardTextureClassifier::is_two_tone_board(const std::array<Card, 3>& board) {
    std::unordered_map<int, int> suit_counts;
    for (const Card& card : board) {
        suit_counts[get_suit(card)]++;
    }
    
    return suit_counts.size() == 2 && 
           std::any_of(suit_counts.begin(), suit_counts.end(), 
                      [](const auto& pair) { return pair.second >= 2; });
}

bool BoardAnalysisCommand::BoardTextureClassifier::is_highly_connected(const std::array<Card, 3>& board) {
    return calculate_connectivity_index(board) >= 0.7;
}

bool BoardAnalysisCommand::BoardTextureClassifier::is_broadway_heavy(const std::array<Card, 3>& board) {
    int broadway_count = 0;
    for (const Card& card : board) {
        if (get_rank(card) >= 10) { // T, J, Q, K, A
            broadway_count++;
        }
    }
    return broadway_count >= 2;
}

bool BoardAnalysisCommand::BoardTextureClassifier::is_wheel_texture(const std::array<Card, 3>& board) {
    std::vector<int> ranks;
    for (const Card& card : board) {
        ranks.push_back(get_rank(card));
    }
    std::sort(ranks.begin(), ranks.end());
    
    // Check for wheel possibilities (A-2-3-4-5)
    bool has_ace = std::any_of(ranks.begin(), ranks.end(), [](int rank) { return rank == 14; });
    bool has_low_cards = std::any_of(ranks.begin(), ranks.end(), [](int rank) { return rank <= 5; });
    
    return has_ace && has_low_cards;
}

// Strategic calculation functions
double BoardAnalysisCommand::BoardTextureClassifier::calculate_expected_cbet_freq(BoardTexture primary_texture) {
    auto it = cbet_frequency_table_.find(primary_texture);
    return (it != cbet_frequency_table_.end()) ? it->second : 0.60; // Default frequency
}

double BoardAnalysisCommand::BoardTextureClassifier::calculate_expected_checkraise_freq(BoardTexture primary_texture) {
    auto it = checkraise_frequency_table_.find(primary_texture);
    return (it != checkraise_frequency_table_.end()) ? it->second : 0.15; // Default frequency
}

double BoardAnalysisCommand::BoardTextureClassifier::calculate_range_advantage_pfr(BoardTexture primary_texture, double connectivity) {
    auto it = range_advantage_table_.find(primary_texture);
    double base_advantage = (it != range_advantage_table_.end()) ? it->second : 0.50;
    
    // Adjust based on connectivity (more connected = less PFR advantage)
    return base_advantage * (1.0 - connectivity * 0.3);
}

std::string BoardAnalysisCommand::BoardTextureClassifier::texture_to_string(BoardTexture texture) const {
    switch (texture) {
        case BoardTexture::DRY_RAINBOW: return "Dry Rainbow";
        case BoardTexture::WET_CONNECTED: return "Wet Connected";
        case BoardTexture::PAIRED: return "Paired";
        case BoardTexture::MONOTONE: return "Monotone";
        case BoardTexture::TWO_TONE: return "Two-Tone";
        case BoardTexture::HIGHLY_CONNECTED: return "Highly Connected";
        case BoardTexture::MODERATELY_CONNECTED: return "Moderately Connected";
        case BoardTexture::DISCONNECTED: return "Disconnected";
        case BoardTexture::HIGH_BOARD: return "High Board";
        case BoardTexture::MIDDLE_BOARD: return "Middle Board";
        case BoardTexture::LOW_BOARD: return "Low Board";
        case BoardTexture::ACE_HIGH_DRY: return "Ace High Dry";
        case BoardTexture::BROADWAY_HEAVY: return "Broadway Heavy";
        case BoardTexture::WHEEL_TEXTURE: return "Wheel Texture";
        default: return "Unknown";
    }
}

// Test and analysis functions
void BoardAnalysisCommand::test_dry_boards() {
    auto dry_boards = get_dry_boards();
    std::vector<BoardAnalysis> analyses;
    
    for (size_t i = 0; i < dry_boards.size(); ++i) {
        show_progress(i + 1, dry_boards.size(), dry_boards[i]);
        
        try {
            BoardAnalysis analysis = classifier_.classify_board_string(dry_boards[i]);
            analyses.push_back(analysis);
        } catch (const std::exception& e) {
            std::cerr << "\nError analyzing board " << dry_boards[i] << ": " << e.what() << std::endl;
        }
    }
    
    export_board_classifications(analyses, "output/board_analysis/dry_boards_analysis.csv");
}

void BoardAnalysisCommand::test_wet_boards() {
    auto wet_boards = get_wet_boards();
    std::vector<BoardAnalysis> analyses;
    
    for (size_t i = 0; i < wet_boards.size(); ++i) {
        show_progress(i + 1, wet_boards.size(), wet_boards[i]);
        
        try {
            BoardAnalysis analysis = classifier_.classify_board_string(wet_boards[i]);
            analyses.push_back(analysis);
        } catch (const std::exception& e) {
            std::cerr << "\nError analyzing board " << wet_boards[i] << ": " << e.what() << std::endl;
        }
    }
    
    export_board_classifications(analyses, "output/board_analysis/wet_boards_analysis.csv");
}

void BoardAnalysisCommand::test_paired_boards() {
    auto paired_boards = get_paired_boards();
    std::vector<BoardAnalysis> analyses;
    
    for (size_t i = 0; i < paired_boards.size(); ++i) {
        show_progress(i + 1, paired_boards.size(), paired_boards[i]);
        
        try {
            BoardAnalysis analysis = classifier_.classify_board_string(paired_boards[i]);
            analyses.push_back(analysis);
        } catch (const std::exception& e) {
            std::cerr << "\nError analyzing board " << paired_boards[i] << ": " << e.what() << std::endl;
        }
    }
    
    export_board_classifications(analyses, "output/board_analysis/paired_boards_analysis.csv");
}

void BoardAnalysisCommand::test_special_cases() {
    std::vector<std::string> special_boards;
    auto monotone = get_monotone_boards();
    auto broadway = get_broadway_boards();
    auto wheel = get_wheel_boards();
    
    special_boards.insert(special_boards.end(), monotone.begin(), monotone.end());
    special_boards.insert(special_boards.end(), broadway.begin(), broadway.end());
    special_boards.insert(special_boards.end(), wheel.begin(), wheel.end());
    
    std::vector<BoardAnalysis> analyses;
    
    for (size_t i = 0; i < special_boards.size(); ++i) {
        show_progress(i + 1, special_boards.size(), special_boards[i]);
        
        try {
            BoardAnalysis analysis = classifier_.classify_board_string(special_boards[i]);
            analyses.push_back(analysis);
        } catch (const std::exception& e) {
            std::cerr << "\nError analyzing board " << special_boards[i] << ": " << e.what() << std::endl;
        }
    }
    
    export_board_classifications(analyses, "output/board_analysis/special_cases_analysis.csv");
}

void BoardAnalysisCommand::analyze_representative_boards() {
    std::vector<std::string> all_boards;
    
    // Collect all representative boards
    auto dry = get_dry_boards();
    auto wet = get_wet_boards(); 
    auto paired = get_paired_boards();
    auto monotone = get_monotone_boards();
    auto broadway = get_broadway_boards();
    auto wheel = get_wheel_boards();
    
    all_boards.insert(all_boards.end(), dry.begin(), dry.end());
    all_boards.insert(all_boards.end(), wet.begin(), wet.end());
    all_boards.insert(all_boards.end(), paired.begin(), paired.end());
    all_boards.insert(all_boards.end(), monotone.begin(), monotone.end());
    all_boards.insert(all_boards.end(), broadway.begin(), broadway.end());
    all_boards.insert(all_boards.end(), wheel.begin(), wheel.end());
    
    std::vector<BoardAnalysis> analyses;
    
    for (size_t i = 0; i < all_boards.size(); ++i) {
        show_progress(i + 1, all_boards.size(), all_boards[i]);
        
        try {
            BoardAnalysis analysis = classifier_.classify_board_string(all_boards[i]);
            analyses.push_back(analysis);
        } catch (const std::exception& e) {
            std::cerr << "\nError analyzing board " << all_boards[i] << ": " << e.what() << std::endl;
        }
    }
    
    export_board_classifications(analyses, "output/board_analysis/comprehensive_board_analysis.csv");
}

void BoardAnalysisCommand::export_board_classifications(const std::vector<BoardAnalysis>& analyses, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file for writing: " + filename);
    }
    
    // CSV header
    file << "board,primary_texture,secondary_texture,connectivity_index,flush_potential,pair_potential,high_card_bias,expected_cbet_freq,expected_checkraise_freq,range_advantage_pfr\n";
    
    for (const auto& analysis : analyses) {
        file << analysis.board_string << ","
             << classifier_.texture_to_string(analysis.primary_texture) << ","
             << classifier_.texture_to_string(analysis.secondary_texture) << ","
             << std::fixed << std::setprecision(3) << analysis.connectivity_index << ","
             << analysis.flush_potential << ","
             << analysis.pair_potential << ","
             << analysis.high_card_bias << ","
             << analysis.expected_cbet_freq << ","
             << analysis.expected_checkraise_freq << ","
             << analysis.range_advantage_pfr << "\n";
    }
    
    file.close();
}

void BoardAnalysisCommand::create_output_directories() {
    try {
        std::filesystem::create_directories("output/board_analysis");
    } catch (const std::exception& e) {
        std::cerr << "Error creating output directories: " << e.what() << std::endl;
        throw;
    }
}

// Representative board collections
std::vector<std::string> BoardAnalysisCommand::get_dry_boards() const {
    return {
        "As7h2c", "Kd8s3h", "Qc6h4d", "Jh5c2s", "Ts4h3c",
        "9s3h2d", "8h4c2s", "7c3s2h", "6d4h2c", "5s3c2d",
        "Ah8s3c", "Kc7h2d", "Qd5s3h", "Js4c2h", "Tc6h3d",
        "9h3s2c", "8s4d3h", "7h2c4s", "6c3d2h", "5h4s2c"
    };
}

std::vector<std::string> BoardAnalysisCommand::get_wet_boards() const {
    return {
        "9s8h7c", "JcTh9s", "8h7c6d", "ThJc9h", "7s6h5c",
        "QcJh9s", "TsJc8h", "9c8h6s", "JhTs7c", "8s7h5d",
        "9s8s7c", "JcTc9h", "8h7h6d", "ThJsKc", "7s6s5h",
        "QsJs9c", "TcJh8s", "9h8c6s", "JsTh7c", "8c7s5d"
    };
}

std::vector<std::string> BoardAnalysisCommand::get_paired_boards() const {
    return {
        "AsAh7c", "KdKs3h", "QhQc8s", "JsJh4c", "TcTh6s",
        "9s9h2c", "8h8c5s", "7c7s3h", "6d6h4c", "5s5c2h",
        "AcAd2s", "KhKc7s", "QsQd4h", "JcJd8s", "ThTs3c",
        "9c9d5h", "8s8d2c", "7h7d6s", "6c6s3d", "5h5d4c"
    };
}

std::vector<std::string> BoardAnalysisCommand::get_monotone_boards() const {
    return {
        "AsKs2s", "QhJh7h", "TcJc9c", "8d7d5d", "6s5s3s",
        "AhQh4h", "KcTc6c", "Js9s2s", "7h6h3h", "5d4d2d"
    };
}

std::vector<std::string> BoardAnalysisCommand::get_broadway_boards() const {
    return {
        "AhKsQc", "KcQhJs", "QsJhTc", "JcThKs", "AsQc9h",
        "KhJs8c", "QcTh7s", "Jh9s6c", "AcKh5s", "QsKd4h"
    };
}

std::vector<std::string> BoardAnalysisCommand::get_wheel_boards() const {
    return {
        "As5h3c", "4h3c2s", "5s4h2c", "Ac4h3s", "3h2c5d",
        "As2h4c", "5c3s2h", "4d2s3h", "Ah5c2s", "3c2h4s"
    };
}

} // namespace poker
