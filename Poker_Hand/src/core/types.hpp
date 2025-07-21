#pragma once

#include <cstdint>
#include <array>
#include <string>
#include <vector>
#include <immintrin.h>

namespace poker {

// Card representation using bit manipulation
// 4 bits for rank (2-14), 4 bits for suit (0-3)
using Card = uint8_t;
using CardPack = uint32_t;  // Pack 4 cards for SIMD

// SIMD types for vectorized operations
using SimdCards = __m256i;
using SimdResults = __m256d;

// Hand evaluation constants
constexpr int HAND_RANKS_SIZE = 32768;
constexpr int FLUSH_RANKS_SIZE = 8192;
constexpr int NUM_CARDS = 52;
constexpr int HAND_SIZE = 2;
constexpr int BOARD_SIZE = 5;

// Poker hand types
enum class HandType : uint8_t {
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

// Table size types
enum class TableSize : uint8_t {
    SIX_MAX = 6,
    NINE_MAX = 9
};

// Position types for 6-max and 9-max
enum class Position : uint8_t {
    // 6-max positions
    UTG_6MAX = 0,
    HJ_6MAX = 1,
    CO_6MAX = 2,
    BTN_6MAX = 3,
    SB_6MAX = 4,
    BB_6MAX = 5,
    
    // 9-max positions
    UTG_9MAX = 6,
    UTG1_9MAX = 7,
    MP1_9MAX = 8,
    MP2_9MAX = 9,
    HJ_9MAX = 10,
    CO_9MAX = 11,
    BTN_9MAX = 12,
    SB_9MAX = 13,
    BB_9MAX = 14,
    
    // Legacy support
    UTG = UTG_6MAX,
    MP = HJ_6MAX,
    CO = CO_6MAX,
    BTN = BTN_6MAX,
    SB = SB_6MAX,
    BB = BB_6MAX
};

// Enhanced action types
enum class PokerAction : uint8_t {
    // Primary actions (prioritize these)
    OPEN_RAISE = 0,          
    THREE_BET = 1,           
    FOUR_BET = 2,            
    CALL = 3,                
    
    // Secondary actions
    FOLD = 4,
    CHECK = 5,
    RE_RAISE = 6,
    FIVE_BET_OR_MORE = 7,
    CONTINUATION_BET = 8,
    DONK_BET = 9,
    CHECK_RAISE = 10,
    ALL_IN = 11
};

// Legacy action support
enum class Action : uint8_t {
    FOLD = 0,
    CALL = 1,
    RAISE = 2,
    THREE_BET = 3,
    FOUR_BET = 4
};

// Board texture classification system
enum class BoardTexture : uint8_t {
    // Primary Classifications
    DRY_RAINBOW = 0,        // A72r, K83r (disconnected, rainbow)
    WET_CONNECTED = 1,      // 987ss, JT9cc (straight/flush draws)
    PAIRED = 2,             // AAx, KKx, 77x (trips potential)
    MONOTONE = 3,           // As7s2s (flush completed)
    TWO_TONE = 4,           // AhKh7c (flush draw present)
    
    // Connectivity Levels  
    HIGHLY_CONNECTED = 5,   // 876, JT9 (multiple straight draws)
    MODERATELY_CONNECTED = 6, // A65, KJ8 (some connectivity)
    DISCONNECTED = 7,       // A72, K62 (minimal connectivity)
    
    // Card Height Classifications
    HIGH_BOARD = 8,         // AKQ, KQJ (premium pairs help)
    MIDDLE_BOARD = 9,       // JT9, 987 (middle pairs/draws)
    LOW_BOARD = 10,         // 765, 432 (low pairs/straight draws)
    
    // Special Cases
    ACE_HIGH_DRY = 11,      // A72r (nut advantage to pfr)
    BROADWAY_HEAVY = 12,    // KQJ, QJT (high card advantage)
    WHEEL_TEXTURE = 13      // A23, 234 (wheel straight potential)
};

// Board analysis structure
struct BoardAnalysis {
    BoardTexture primary_texture;
    BoardTexture secondary_texture;
    
    // Quantified metrics (0.0 - 1.0)
    double connectivity_index;    // How connected (straight draws)
    double flush_potential;       // Flush draw strength
    double pair_potential;        // Set/two-pair potential  
    double high_card_bias;        // Advantage to high cards
    
    // Strategic implications
    double expected_cbet_freq;    // Based on texture
    double expected_checkraise_freq;
    double range_advantage_pfr;   // How much PFR benefits
    
    // Board string representation
    std::string board_string;     // e.g., "As7h2c"
    
    BoardAnalysis() : primary_texture(BoardTexture::DRY_RAINBOW), 
                     secondary_texture(BoardTexture::DRY_RAINBOW),
                     connectivity_index(0.0), flush_potential(0.0), 
                     pair_potential(0.0), high_card_bias(0.0),
                     expected_cbet_freq(0.0), expected_checkraise_freq(0.0),
                     range_advantage_pfr(0.0), board_string("") {}
};

// Flop action analysis structure
struct FlopActionResult {
    std::string hand;                    // e.g., "AA", "KQs", "72o"
    std::string position;                // UTG, HJ, CO, BTN, SB, BB
    std::string preflop_action;          // open, 3bet, 4bet, call
    BoardAnalysis board_analysis;        // Complete board classification
    std::string flop_action;             // cbet, check, check_raise, call, fold
    double action_frequency;             // How often this action is taken
    double win_rate_after_action;        // Win rate if taking this action
    double expected_value;               // EV of the action
    uint64_t simulations_run;            // Monte Carlo simulations
    
    FlopActionResult() : hand(""), position(""), preflop_action(""),
                        flop_action(""), action_frequency(0.0),
                        win_rate_after_action(0.0), expected_value(0.0),
                        simulations_run(0) {}
};

// Simulation result structure
struct SimulationResult {
    double win_rate;
    double tie_rate;
    double loss_rate;
    uint64_t total_simulations;
    double confidence_interval_low;
    double confidence_interval_high;
    double expected_value;
    
    SimulationResult() : win_rate(0.0), tie_rate(0.0), loss_rate(0.0), 
                        total_simulations(0), confidence_interval_low(0.0), 
                        confidence_interval_high(0.0), expected_value(0.0) {}
};

// Hand analysis structure
struct HandAnalysis {
    std::string hand_string;
    int combinations;
    double equity_vs_random;
    double equity_vs_tight;
    double equity_vs_loose;
    
    std::array<SimulationResult, 9> vs_opponents;  // 1-9 opponents
    std::array<Action, 6> position_actions;        // Actions for each position
    std::array<double, 6> position_frequencies;    // Frequency for each position
    std::array<double, 6> position_ev;             // Expected value for each position
    
    HandAnalysis() : combinations(0), equity_vs_random(0.0), equity_vs_tight(0.0), 
                    equity_vs_loose(0.0) {
        position_actions.fill(Action::FOLD);
        position_frequencies.fill(0.0);
        position_ev.fill(0.0);
    }
};

// Action result structure for CSV export
struct ActionResult {
    std::string hand;                    // e.g., "AA", "KQs", "72o"
    std::string position;                // UTG, UTG1, MP1, MP2, HJ, CO, BTN, SB, BB
    std::string action;                  // opening_raise, 3_bet, 4_bet, call
    double win_rate;                     // decimal, e.g., 0.653 for 65.3%
    int player_count;                    // 6 or 9
    uint64_t simulations_run;            // number of Monte Carlo simulations
    double expected_value;               // EV calculation
    double confidence_interval_low;      // confidence interval lower bound
    double confidence_interval_high;     // confidence interval upper bound
    bool in_range;                       // true if hand is in the standard opening range for that position
    
    ActionResult() : hand(""), position(""), action(""), win_rate(0.0), 
                    player_count(6), simulations_run(0), expected_value(0.0),
                    confidence_interval_low(0.0), confidence_interval_high(0.0), 
                    in_range(false) {}
};

// Performance statistics
struct PerformanceStats {
    uint64_t total_simulations;
    double simulations_per_second;
    double total_time_seconds;
    uint64_t cache_hits;
    uint64_t cache_misses;
    double cache_hit_rate;
    
    PerformanceStats() : total_simulations(0), simulations_per_second(0.0), 
                        total_time_seconds(0.0), cache_hits(0), cache_misses(0), 
                        cache_hit_rate(0.0) {}
};

// Thread-local data for parallel processing
struct alignas(64) ThreadLocalData {
    std::array<Card, NUM_CARDS> deck;
    std::array<Card, HAND_SIZE> hero_hand;
    std::array<Card, BOARD_SIZE> board;
    std::array<std::array<Card, HAND_SIZE>, 9> opponent_hands;
    uint64_t win_count;
    uint64_t tie_count;
    uint64_t total_count;
    
    ThreadLocalData() : win_count(0), tie_count(0), total_count(0) {}
};

// Configuration structure
struct Config {
    int min_simulations_per_hand;
    int max_simulations_per_hand;
    int num_threads;
    bool use_simd;
    bool use_cache;
    double confidence_level;
    
    Config() : min_simulations_per_hand(100000), max_simulations_per_hand(10000000),
               num_threads(0), use_simd(true), use_cache(true), confidence_level(0.95) {}
};

// Utility functions for card operations
inline Card make_card(int rank, int suit) {
    return static_cast<Card>((rank << 4) | suit);
}

inline int get_rank(Card card) {
    return (card >> 4) & 0x0F;
}

inline int get_suit(Card card) {
    return card & 0x0F;
}

inline std::string card_to_string(Card card) {
    const char* ranks = "23456789TJQKA";
    const char* suits = "cdhs";
    return std::string(1, ranks[get_rank(card) - 2]) + std::string(1, suits[get_suit(card)]);
}

// Hand string parsing
std::pair<Card, Card> parse_hand_string(const std::string& hand_str);
std::string hand_to_string(Card card1, Card card2);

// SIMD utility functions
inline SimdCards pack_cards_simd(const std::array<Card, 8>& cards) {
    return _mm256_loadu_si256(reinterpret_cast<const __m256i*>(cards.data()));
}

inline void unpack_cards_simd(const SimdCards& packed, std::array<Card, 8>& cards) {
    _mm256_storeu_si256(reinterpret_cast<__m256i*>(cards.data()), packed);
}

} // namespace poker
