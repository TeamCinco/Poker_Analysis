#pragma once

#include <string>
#include <vector>
#include <array>

namespace poker {

// Basic card representation
enum class Suit { CLUBS = 0, DIAMONDS = 1, HEARTS = 2, SPADES = 3 };
enum class Rank { TWO = 2, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN, JACK, QUEEN, KING, ACE };

struct Card {
    Rank rank;
    Suit suit;
    
    Card() = default;
    Card(Rank r, Suit s) : rank(r), suit(s) {}
    
    bool operator==(const Card& other) const {
        return rank == other.rank && suit == other.suit;
    }
    
    std::string to_string() const;
};

// Position enum
enum class Position {
    UTG = 0,
    HJ = 1,    // Hijack
    CO = 2,    // Cutoff  
    BTN = 3,   // Button
    SB = 4,    // Small Blind
    BB = 5     // Big Blind
};

std::string position_to_string(Position pos);
Position string_to_position(const std::string& pos_str);

// Street enum
enum class Street {
    PREFLOP = 0,
    FLOP = 1,
    TURN = 2,
    RIVER = 3
};

std::string street_to_string(Street street);

// Core data structures
struct Scenario {
    std::string hand;                    // "AA", "KQs", etc.
    Position position;                   // UTG, BTN, etc.  
    std::vector<std::string> actions;    // ["open", "3bet", "call"]
    std::string board;                   // "As7h2c" (if postflop)
    Street street;                       // preflop, flop, turn, river
    int table_size;                      // 6, 7, 8, or 9
    int num_opponents;                   // table_size - 1
    
    std::string get_action_sequence() const;
};

struct SimResult {
    double optimal_frequency;     // 0.0 to 1.0
    double expected_value;        // EV at optimal frequency
    int simulations_run;
    double confidence_interval;
    int table_size;
    
    SimResult() : optimal_frequency(0.0), expected_value(0.0), 
                  simulations_run(0), confidence_interval(0.0), table_size(6) {}
};

// Hand evaluation result
enum class HandRank {
    HIGH_CARD = 0,
    PAIR = 1,
    TWO_PAIR = 2,
    THREE_OF_A_KIND = 3,
    STRAIGHT = 4,
    FLUSH = 5,
    FULL_HOUSE = 6,
    FOUR_OF_A_KIND = 7,
    STRAIGHT_FLUSH = 8,
    ROYAL_FLUSH = 9
};

struct HandValue {
    HandRank rank;
    int strength;  // Numeric strength for comparison
    
    bool operator<(const HandValue& other) const {
        return strength < other.strength;
    }
    
    bool operator>(const HandValue& other) const {
        return strength > other.strength;
    }
    
    bool operator==(const HandValue& other) const {
        return strength == other.strength;
    }
};

} // namespace poker
