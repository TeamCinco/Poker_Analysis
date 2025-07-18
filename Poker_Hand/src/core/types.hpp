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

// Position types
enum class Position : uint8_t {
    UTG = 0,
    MP = 1,
    CO = 2,
    BTN = 3,
    SB = 4,
    BB = 5
};

// Action types
enum class Action : uint8_t {
    FOLD = 0,
    CALL = 1,
    RAISE = 2,
    THREE_BET = 3,
    FOUR_BET = 4
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
