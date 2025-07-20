#pragma once

#include "types.hpp"
#include <vector>
#include <string>
#include <unordered_map>

namespace poker {

class RealisticRanges {
public:
    // Initialize all ranges
    static void initialize();
    
    // Get opening range for position and table size
    static std::vector<std::string> get_opening_range(const std::string& position, TableSize table_size);
    
    // Get 3-bet range for position and table size  
    static std::vector<std::string> get_3bet_range(const std::string& position, TableSize table_size);
    
    // Get 4-bet range for position and table size
    static std::vector<std::string> get_4bet_range(const std::string& position, TableSize table_size);
    
    // Check if hand is in range
    static bool is_hand_in_range(const std::string& hand, const std::string& position, 
                               TableSize table_size, PokerAction action);
    
    // Get all position names for table size
    static std::vector<std::string> get_position_names(TableSize table_size);

private:
    static std::unordered_map<std::string, std::vector<std::string>> opening_ranges_6max_;
    static std::unordered_map<std::string, std::vector<std::string>> opening_ranges_9max_;
    static std::unordered_map<std::string, std::vector<std::string>> threebet_ranges_6max_;
    static std::unordered_map<std::string, std::vector<std::string>> threebet_ranges_9max_;
    static std::unordered_map<std::string, std::vector<std::string>> fourbet_ranges_6max_;
    static std::unordered_map<std::string, std::vector<std::string>> fourbet_ranges_9max_;
    
    static bool ranges_initialized_;
    
    // Initialize specific range types
    static void initialize_6max_opening_ranges();
    static void initialize_9max_opening_ranges();
    static void initialize_3bet_ranges();
    static void initialize_4bet_ranges();
};

// 6-Max Opening Ranges (exactly as specified)
const std::vector<std::string> UTG_6MAX_OPENING = {
    // Pairs
    "77", "88", "99", "TT", "JJ", "QQ", "KK", "AA",
    // Offsuit broadways
    "AJo", "AQo", "AKo", "KQo",
    // Suited hands
    "ATs", "AJs", "AQs", "AKs", "KTs", "KJs", "KQs", "QTs", "QJs", "JTs",
    // Suited wheel aces
    "A5s", "A4s"
};

const std::vector<std::string> HJ_6MAX_OPENING = {
    // Pairs
    "55", "66", "77", "88", "99", "TT", "JJ", "QQ", "KK", "AA",
    // Offsuit broadways
    "ATo", "AJo", "AQo", "AKo", "KJo", "KQo", "QJo",
    // Suited hands
    "A9s", "ATs", "AJs", "AQs", "AKs", "K9s", "KTs", "KJs", "KQs",
    "Q9s", "QTs", "QJs", "J9s", "JTs", "T9s", "98s", "87s"
};

const std::vector<std::string> CO_6MAX_OPENING = {
    // Pairs
    "44", "55", "66", "77", "88", "99", "TT", "JJ", "QQ", "KK", "AA",
    // Offsuit broadways
    "A9o", "ATo", "AJo", "AQo", "AKo", "KTo", "KJo", "KQo", "QTo", "QJo", "JTo",
    // Suited hands
    "A2s", "A3s", "A4s", "A5s", "A6s", "A7s", "A8s", "A9s", "ATs", "AJs", "AQs", "AKs",
    "K7s", "K8s", "K9s", "KTs", "KJs", "KQs", "Q8s", "Q9s", "QTs", "QJs",
    "J8s", "J9s", "JTs", "T8s", "T9s", "97s", "98s", "86s", "87s", "75s", "76s",
    "65s", "54s"
};

const std::vector<std::string> BTN_6MAX_OPENING = {
    // Any pair
    "22", "33", "44", "55", "66", "77", "88", "99", "TT", "JJ", "QQ", "KK", "AA",
    // Any Ace
    "A2o", "A3o", "A4o", "A5o", "A6o", "A7o", "A8o", "A9o", "ATo", "AJo", "AQo", "AKo",
    // Offsuit broadways
    "K9o", "KTo", "KJo", "KQo", "Q9o", "QTo", "QJo", "J9o", "JTo", "T9o",
    // All suited hands
    "A2s", "A3s", "A4s", "A5s", "A6s", "A7s", "A8s", "A9s", "ATs", "AJs", "AQs", "AKs",
    "K2s", "K3s", "K4s", "K5s", "K6s", "K7s", "K8s", "K9s", "KTs", "KJs", "KQs",
    "Q5s", "Q6s", "Q7s", "Q8s", "Q9s", "QTs", "QJs", "J7s", "J8s", "J9s", "JTs",
    "T7s", "T8s", "T9s", "96s", "97s", "98s", "85s", "86s", "87s", "74s", "75s", "76s",
    "64s", "65s", "54s", "53s", "43s", "52s"
};

const std::vector<std::string> SB_6MAX_OPENING = {
    // Pairs
    "66", "77", "88", "99", "TT", "JJ", "QQ", "KK", "AA",
    // Offsuit broadways
    "ATo", "AJo", "AQo", "AKo", "KQo",
    // Suited hands
    "A2s", "A3s", "A4s", "A5s", "A6s", "A7s", "A8s", "A9s", "ATs", "AJs", "AQs", "AKs",
    "K9s", "KTs", "KJs", "KQs", "Q9s", "QTs", "QJs", "J9s", "JTs", "T9s",
    // Occasional suited connectors
    "98s", "87s", "76s", "65s"
};

// 9-Max Opening Ranges (exactly as specified)
const std::vector<std::string> UTG_9MAX_OPENING = {
    "88", "99", "TT", "JJ", "QQ", "KK", "AA",
    "AQo", "AKo", "KQo",
    "AJs", "AQs", "AKs", "KQs"
};

const std::vector<std::string> UTG1_9MAX_OPENING = {
    "77", "88", "99", "TT", "JJ", "QQ", "KK", "AA",
    "AJo", "AQo", "AKo", "KQo",
    "ATs", "AJs", "AQs", "AKs", "KTs", "KJs", "KQs", "QTs", "QJs", "JTs"
};

const std::vector<std::string> MP1_9MAX_OPENING = {
    "66", "77", "88", "99", "TT", "JJ", "QQ", "KK", "AA",
    "AJo", "AQo", "AKo", "KQo", "QJo",
    "A9s", "ATs", "AJs", "AQs", "AKs", "K9s", "KTs", "KJs", "KQs",
    "Q9s", "QTs", "QJs", "J9s", "JTs", "T9s", "98s"
};

const std::vector<std::string> MP2_9MAX_OPENING = {
    "55", "66", "77", "88", "99", "TT", "JJ", "QQ", "KK", "AA",
    "ATo", "AJo", "AQo", "AKo", "KJo", "KQo", "QJo",
    "A8s", "A9s", "ATs", "AJs", "AQs", "AKs", "K8s", "K9s", "KTs", "KJs", "KQs",
    "Q9s", "QTs", "QJs", "J9s", "JTs", "T9s", "98s", "87s"
};

const std::vector<std::string> HJ_9MAX_OPENING = {
    "44", "55", "66", "77", "88", "99", "TT", "JJ", "QQ", "KK", "AA",
    "A9o", "ATo", "AJo", "AQo", "AKo", "KTo", "KJo", "KQo", "QTo", "QJo", "JTo",
    "A5s", "A6s", "A7s", "A8s", "A9s", "ATs", "AJs", "AQs", "AKs",
    "K7s", "K8s", "K9s", "KTs", "KJs", "KQs", "Q8s", "Q9s", "QTs", "QJs",
    "J8s", "J9s", "JTs", "T8s", "T9s", "97s", "98s", "87s", "76s"
};

const std::vector<std::string> CO_9MAX_OPENING = {
    "33", "44", "55", "66", "77", "88", "99", "TT", "JJ", "QQ", "KK", "AA",
    "A7o", "A8o", "A9o", "ATo", "AJo", "AQo", "AKo", "K9o", "KTo", "KJo", "KQo",
    "Q9o", "QTo", "QJo", "J9o", "JTo", "T9o",
    "A2s", "A3s", "A4s", "A5s", "A6s", "A7s", "A8s", "A9s", "ATs", "AJs", "AQs", "AKs",
    "K5s", "K6s", "K7s", "K8s", "K9s", "KTs", "KJs", "KQs", "Q7s", "Q8s", "Q9s", "QTs", "QJs",
    "J7s", "J8s", "J9s", "JTs", "T7s", "T8s", "T9s", "96s", "97s", "98s",
    "86s", "87s", "75s", "76s", "65s", "54s"
};

const std::vector<std::string> BTN_9MAX_OPENING = {
    // Any pair
    "22", "33", "44", "55", "66", "77", "88", "99", "TT", "JJ", "QQ", "KK", "AA",
    // Any Ace and King
    "A2o", "A3o", "A4o", "A5o", "A6o", "A7o", "A8o", "A9o", "ATo", "AJo", "AQo", "AKo",
    "K2o", "K3o", "K4o", "K5o", "K6o", "K7o", "K8o", "K9o", "KTo", "KJo", "KQo",
    // Other offsuit
    "Q8o", "Q9o", "QTo", "QJo", "J8o", "J9o", "JTo", "T8o", "T9o", "98o", "87o",
    // All suited combinations
    "A2s", "A3s", "A4s", "A5s", "A6s", "A7s", "A8s", "A9s", "ATs", "AJs", "AQs", "AKs",
    "K2s", "K3s", "K4s", "K5s", "K6s", "K7s", "K8s", "K9s", "KTs", "KJs", "KQs",
    "Q5s", "Q6s", "Q7s", "Q8s", "Q9s", "QTs", "QJs", "J7s", "J8s", "J9s", "JTs",
    "T7s", "T8s", "T9s", "96s", "97s", "98s", "85s", "86s", "87s", "74s", "75s", "76s",
    "64s", "65s", "53s", "54s", "43s"
};

const std::vector<std::string> SB_9MAX_OPENING = {
    "66", "77", "88", "99", "TT", "JJ", "QQ", "KK", "AA",
    "ATo", "AJo", "AQo", "AKo", "KQo",
    "A2s", "A3s", "A4s", "A5s", "A6s", "A7s", "A8s", "A9s", "ATs", "AJs", "AQs", "AKs",
    "K9s", "KTs", "KJs", "KQs", "Q9s", "QTs", "QJs", "J9s", "JTs", "T9s", "98s",
    "87s", "76s"
};

} // namespace poker
