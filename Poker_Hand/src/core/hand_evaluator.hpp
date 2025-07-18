#pragma once

#include "types.hpp"
#include <array>
#include <cstdint>

namespace poker {

class HandEvaluator {
public:
    HandEvaluator();
    
    // Evaluate a 5-card hand and return its rank
    uint32_t evaluate_5_cards(const std::array<Card, 5>& cards) const;
    
    // Evaluate a 7-card hand (2 hole cards + 5 board cards)
    uint32_t evaluate_7_cards(const std::array<Card, 7>& cards) const;
    
    // SIMD-optimized evaluation for multiple hands
    void evaluate_hands_simd(const std::array<std::array<Card, 5>, 8>& hands, 
                            std::array<uint32_t, 8>& results) const;
    
    // Compare two hands (returns -1, 0, 1 for less, equal, greater)
    int compare_hands(uint32_t hand1_rank, uint32_t hand2_rank) const;
    
    // Get hand type from rank
    HandType get_hand_type(uint32_t rank) const;
    
    // Get hand strength as a percentage (0-100)
    double get_hand_strength(uint32_t rank) const;

private:
    // Precomputed lookup tables for fast evaluation
    std::array<uint32_t, HAND_RANKS_SIZE> hand_ranks_;
    std::array<uint32_t, FLUSH_RANKS_SIZE> flush_ranks_;
    std::array<uint32_t, 8192> unique_ranks_;
    
    // Suit masks for flush detection
    std::array<uint32_t, 4> suit_masks_;
    
    // Initialize lookup tables
    void initialize_tables();
    void initialize_hand_ranks();
    void initialize_flush_ranks();
    void initialize_unique_ranks();
    
    // Helper functions for hand evaluation
    uint32_t evaluate_flush(const std::array<Card, 5>& cards) const;
    uint32_t evaluate_non_flush(const std::array<Card, 5>& cards) const;
    uint32_t find_best_5_from_7(const std::array<Card, 7>& cards) const;
    
    // Bit manipulation utilities
    uint32_t get_rank_mask(const std::array<Card, 5>& cards) const;
    uint32_t get_suit_mask(const std::array<Card, 5>& cards) const;
    bool is_flush(const std::array<Card, 5>& cards) const;
    bool is_straight(uint32_t rank_mask) const;
    
    // SIMD helper functions
    void prepare_simd_data(const std::array<std::array<Card, 5>, 8>& hands,
                          std::array<uint32_t, 8>& rank_masks,
                          std::array<uint32_t, 8>& suit_masks) const;
};

// Global instance for fast access
extern HandEvaluator g_hand_evaluator;

} // namespace poker
