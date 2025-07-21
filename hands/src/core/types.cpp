#include "types.hpp"
#include <sstream>

namespace poker {

std::string Card::to_string() const {
    std::string rank_str;
    switch (rank) {
        case Rank::TWO: rank_str = "2"; break;
        case Rank::THREE: rank_str = "3"; break;
        case Rank::FOUR: rank_str = "4"; break;
        case Rank::FIVE: rank_str = "5"; break;
        case Rank::SIX: rank_str = "6"; break;
        case Rank::SEVEN: rank_str = "7"; break;
        case Rank::EIGHT: rank_str = "8"; break;
        case Rank::NINE: rank_str = "9"; break;
        case Rank::TEN: rank_str = "T"; break;
        case Rank::JACK: rank_str = "J"; break;
        case Rank::QUEEN: rank_str = "Q"; break;
        case Rank::KING: rank_str = "K"; break;
        case Rank::ACE: rank_str = "A"; break;
    }
    
    std::string suit_str;
    switch (suit) {
        case Suit::CLUBS: suit_str = "c"; break;
        case Suit::DIAMONDS: suit_str = "d"; break;
        case Suit::HEARTS: suit_str = "h"; break;
        case Suit::SPADES: suit_str = "s"; break;
    }
    
    return rank_str + suit_str;
}

std::string position_to_string(Position pos) {
    switch (pos) {
        case Position::UTG: return "UTG";
        case Position::HJ: return "HJ";
        case Position::CO: return "CO";
        case Position::BTN: return "BTN";
        case Position::SB: return "SB";
        case Position::BB: return "BB";
    }
    return "UNKNOWN";
}

Position string_to_position(const std::string& pos_str) {
    if (pos_str == "UTG") return Position::UTG;
    if (pos_str == "HJ") return Position::HJ;
    if (pos_str == "CO") return Position::CO;
    if (pos_str == "BTN") return Position::BTN;
    if (pos_str == "SB") return Position::SB;
    if (pos_str == "BB") return Position::BB;
    return Position::UTG; // Default
}

std::string street_to_string(Street street) {
    switch (street) {
        case Street::PREFLOP: return "preflop";
        case Street::FLOP: return "flop";
        case Street::TURN: return "turn";
        case Street::RIVER: return "river";
    }
    return "unknown";
}

std::string Scenario::get_action_sequence() const {
    if (actions.empty()) return "";
    
    std::stringstream ss;
    for (size_t i = 0; i < actions.size(); ++i) {
        if (i > 0) ss << "_";
        ss << actions[i];
    }
    return ss.str();
}

} // namespace poker
