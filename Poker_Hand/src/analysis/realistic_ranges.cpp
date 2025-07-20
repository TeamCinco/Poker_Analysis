#include "realistic_ranges.hpp"
#include <algorithm>

namespace poker {

// Static member initialization
std::unordered_map<std::string, std::vector<std::string>> RealisticRanges::opening_ranges_6max_;
std::unordered_map<std::string, std::vector<std::string>> RealisticRanges::opening_ranges_9max_;
std::unordered_map<std::string, std::vector<std::string>> RealisticRanges::threebet_ranges_6max_;
std::unordered_map<std::string, std::vector<std::string>> RealisticRanges::threebet_ranges_9max_;
std::unordered_map<std::string, std::vector<std::string>> RealisticRanges::fourbet_ranges_6max_;
std::unordered_map<std::string, std::vector<std::string>> RealisticRanges::fourbet_ranges_9max_;
bool RealisticRanges::ranges_initialized_ = false;

void RealisticRanges::initialize() {
    if (ranges_initialized_) {
        return;
    }
    
    initialize_6max_opening_ranges();
    initialize_9max_opening_ranges();
    initialize_3bet_ranges();
    initialize_4bet_ranges();
    
    ranges_initialized_ = true;
}

void RealisticRanges::initialize_6max_opening_ranges() {
    opening_ranges_6max_["UTG"] = UTG_6MAX_OPENING;
    opening_ranges_6max_["HJ"] = HJ_6MAX_OPENING;
    opening_ranges_6max_["CO"] = CO_6MAX_OPENING;
    opening_ranges_6max_["BTN"] = BTN_6MAX_OPENING;
    opening_ranges_6max_["SB"] = SB_6MAX_OPENING;
    
    // BB typically calls more than opens, so use a tighter range for opening
    opening_ranges_6max_["BB"] = {
        "77", "88", "99", "TT", "JJ", "QQ", "KK", "AA",
        "ATo", "AJo", "AQo", "AKo", "KQo",
        "A8s", "A9s", "ATs", "AJs", "AQs", "AKs", "K9s", "KTs", "KJs", "KQs",
        "Q9s", "QTs", "QJs", "J9s", "JTs", "T9s", "98s"
    };
}

void RealisticRanges::initialize_9max_opening_ranges() {
    opening_ranges_9max_["UTG"] = UTG_9MAX_OPENING;
    opening_ranges_9max_["UTG1"] = UTG1_9MAX_OPENING;
    opening_ranges_9max_["MP1"] = MP1_9MAX_OPENING;
    opening_ranges_9max_["MP2"] = MP2_9MAX_OPENING;
    opening_ranges_9max_["HJ"] = HJ_9MAX_OPENING;
    opening_ranges_9max_["CO"] = CO_9MAX_OPENING;
    opening_ranges_9max_["BTN"] = BTN_9MAX_OPENING;
    opening_ranges_9max_["SB"] = SB_9MAX_OPENING;
    
    // BB opening range for 9-max
    opening_ranges_9max_["BB"] = {
        "77", "88", "99", "TT", "JJ", "QQ", "KK", "AA",
        "ATo", "AJo", "AQo", "AKo", "KQo",
        "A8s", "A9s", "ATs", "AJs", "AQs", "AKs", "K9s", "KTs", "KJs", "KQs",
        "Q9s", "QTs", "QJs", "J9s", "JTs", "T9s", "98s"
    };
}

void RealisticRanges::initialize_3bet_ranges() {
    // 6-max 3-bet ranges (tighter than opening ranges)
    threebet_ranges_6max_["UTG"] = {
        "TT", "JJ", "QQ", "KK", "AA", "AKs", "AKo", "AQs"
    };
    
    threebet_ranges_6max_["HJ"] = {
        "99", "TT", "JJ", "QQ", "KK", "AA", "AKs", "AKo", "AQs", "AJs", "KQs"
    };
    
    threebet_ranges_6max_["CO"] = {
        "88", "99", "TT", "JJ", "QQ", "KK", "AA", 
        "AKs", "AKo", "AQs", "AQo", "AJs", "ATs", "KQs", "KJs",
        "A5s", "A4s"  // Some bluffs
    };
    
    threebet_ranges_6max_["BTN"] = {
        "77", "88", "99", "TT", "JJ", "QQ", "KK", "AA",
        "AKs", "AKo", "AQs", "AQo", "AJs", "AJo", "ATs", "A9s",
        "KQs", "KQo", "KJs", "QJs",
        "A5s", "A4s", "A3s", "A2s"  // More bluffs on button
    };
    
    threebet_ranges_6max_["SB"] = {
        "88", "99", "TT", "JJ", "QQ", "KK", "AA",
        "AKs", "AKo", "AQs", "AQo", "AJs", "ATs", "KQs",
        "A5s", "A4s"
    };
    
    threebet_ranges_6max_["BB"] = {
        "88", "99", "TT", "JJ", "QQ", "KK", "AA",
        "AKs", "AKo", "AQs", "AQo", "AJs", "ATs", "KQs", "KJs",
        "A5s", "A4s", "A3s"
    };

    // 9-max 3-bet ranges (generally tighter than 6-max)
    threebet_ranges_9max_["UTG"] = {
        "JJ", "QQ", "KK", "AA", "AKs", "AKo"
    };
    
    threebet_ranges_9max_["UTG1"] = {
        "TT", "JJ", "QQ", "KK", "AA", "AKs", "AKo", "AQs"
    };
    
    threebet_ranges_9max_["MP1"] = {
        "99", "TT", "JJ", "QQ", "KK", "AA", "AKs", "AKo", "AQs", "AJs"
    };
    
    threebet_ranges_9max_["MP2"] = {
        "88", "99", "TT", "JJ", "QQ", "KK", "AA", "AKs", "AKo", "AQs", "AJs", "KQs"
    };
    
    threebet_ranges_9max_["HJ"] = {
        "88", "99", "TT", "JJ", "QQ", "KK", "AA", 
        "AKs", "AKo", "AQs", "AQo", "AJs", "ATs", "KQs", "KJs"
    };
    
    threebet_ranges_9max_["CO"] = {
        "77", "88", "99", "TT", "JJ", "QQ", "KK", "AA",
        "AKs", "AKo", "AQs", "AQo", "AJs", "AJo", "ATs", "KQs", "KJs",
        "A5s", "A4s"
    };
    
    threebet_ranges_9max_["BTN"] = {
        "66", "77", "88", "99", "TT", "JJ", "QQ", "KK", "AA",
        "AKs", "AKo", "AQs", "AQo", "AJs", "AJo", "ATs", "A9s",
        "KQs", "KQo", "KJs", "QJs",
        "A5s", "A4s", "A3s", "A2s"
    };
    
    threebet_ranges_9max_["SB"] = {
        "77", "88", "99", "TT", "JJ", "QQ", "KK", "AA",
        "AKs", "AKo", "AQs", "AQo", "AJs", "ATs", "KQs",
        "A5s", "A4s"
    };
    
    threebet_ranges_9max_["BB"] = {
        "77", "88", "99", "TT", "JJ", "QQ", "KK", "AA",
        "AKs", "AKo", "AQs", "AQo", "AJs", "ATs", "KQs", "KJs",
        "A5s", "A4s", "A3s"
    };
}

void RealisticRanges::initialize_4bet_ranges() {
    // 6-max 4-bet ranges (very tight, mostly value with some bluffs)
    std::vector<std::string> standard_4bet_6max = {
        "QQ", "KK", "AA", "AKs", "AKo", "A5s"  // A5s as bluff
    };
    
    std::vector<std::string> btn_4bet_6max = {
        "JJ", "QQ", "KK", "AA", "AKs", "AKo", "A5s", "A4s"  // More bluffs on button
    };
    
    std::vector<std::string> positions_6max = {"UTG", "HJ", "CO", "SB", "BB"};
    for (const auto& pos : positions_6max) {
        fourbet_ranges_6max_[pos] = standard_4bet_6max;
    }
    fourbet_ranges_6max_["BTN"] = btn_4bet_6max;

    // 9-max 4-bet ranges (even tighter than 6-max)
    std::vector<std::string> standard_4bet_9max = {
        "KK", "AA", "AKs", "AKo"
    };
    
    std::vector<std::string> late_pos_4bet_9max = {
        "QQ", "KK", "AA", "AKs", "AKo", "A5s"
    };
    
    std::vector<std::string> early_positions_9max = {"UTG", "UTG1", "MP1", "MP2"};
    for (const auto& pos : early_positions_9max) {
        fourbet_ranges_9max_[pos] = standard_4bet_9max;
    }
    
    std::vector<std::string> late_positions_9max = {"HJ", "CO", "BTN", "SB", "BB"};
    for (const auto& pos : late_positions_9max) {
        fourbet_ranges_9max_[pos] = late_pos_4bet_9max;
    }
}

std::vector<std::string> RealisticRanges::get_opening_range(const std::string& position, TableSize table_size) {
    initialize();
    
    if (table_size == TableSize::SIX_MAX) {
        auto it = opening_ranges_6max_.find(position);
        if (it != opening_ranges_6max_.end()) {
            return it->second;
        }
    } else if (table_size == TableSize::NINE_MAX) {
        auto it = opening_ranges_9max_.find(position);
        if (it != opening_ranges_9max_.end()) {
            return it->second;
        }
    }
    
    return {};  // Return empty range if position not found
}

std::vector<std::string> RealisticRanges::get_3bet_range(const std::string& position, TableSize table_size) {
    initialize();
    
    if (table_size == TableSize::SIX_MAX) {
        auto it = threebet_ranges_6max_.find(position);
        if (it != threebet_ranges_6max_.end()) {
            return it->second;
        }
    } else if (table_size == TableSize::NINE_MAX) {
        auto it = threebet_ranges_9max_.find(position);
        if (it != threebet_ranges_9max_.end()) {
            return it->second;
        }
    }
    
    return {};
}

std::vector<std::string> RealisticRanges::get_4bet_range(const std::string& position, TableSize table_size) {
    initialize();
    
    if (table_size == TableSize::SIX_MAX) {
        auto it = fourbet_ranges_6max_.find(position);
        if (it != fourbet_ranges_6max_.end()) {
            return it->second;
        }
    } else if (table_size == TableSize::NINE_MAX) {
        auto it = fourbet_ranges_9max_.find(position);
        if (it != fourbet_ranges_9max_.end()) {
            return it->second;
        }
    }
    
    return {};
}

bool RealisticRanges::is_hand_in_range(const std::string& hand, const std::string& position, 
                                     TableSize table_size, PokerAction action) {
    std::vector<std::string> range;
    
    switch (action) {
        case PokerAction::OPEN_RAISE:
            range = get_opening_range(position, table_size);
            break;
        case PokerAction::THREE_BET:
            range = get_3bet_range(position, table_size);
            break;
        case PokerAction::FOUR_BET:
            range = get_4bet_range(position, table_size);
            break;
        default:
            return false;  // Unknown action
    }
    
    return std::find(range.begin(), range.end(), hand) != range.end();
}

std::vector<std::string> RealisticRanges::get_position_names(TableSize table_size) {
    if (table_size == TableSize::SIX_MAX) {
        return {"UTG", "HJ", "CO", "BTN", "SB", "BB"};
    } else if (table_size == TableSize::NINE_MAX) {
        return {"UTG", "UTG1", "MP1", "MP2", "HJ", "CO", "BTN", "SB", "BB"};
    }
    
    return {};
}

} // namespace poker
