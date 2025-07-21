#pragma once

#include "types.hpp"
#include <vector>
#include <array>

namespace poker {

class HandEvaluator {
public:
    // Evaluate 5-card hand
    static HandValue evaluate_hand(const std::array<Card, 5>& cards);
    
    // Evaluate best 5-card hand from 7 cards (hole cards + board)
    static HandValue evaluate_best_hand(const std::array<Card, 2>& hole_cards, 
                                       const std::vector<Card>& board);
    
    // Compare two hands (returns 1 if hand1 wins, -1 if hand2 wins, 0 if tie)
    static int compare_hands(const HandValue& hand1, const HandValue& hand2);

private:
    // Helper functions for hand evaluation
    static bool is_flush(const std::array<Card, 5>& cards);
    static bool is_straight(const std::array<Card, 5>& cards);
    static std::vector<int> get_rank_counts(const std::array<Card, 5>& cards);
    
    // Calculate hand strength (higher is better)
    static int calculate_strength(HandRank rank, const std::array<Card, 5>& cards);
    
    // Generate all possible 5-card combinations from 7 cards
    static std::vector<std::array<Card, 5>> get_all_combinations(
        const std::array<Card, 2>& hole_cards, const std::vector<Card>& board);
    
    // Sort cards by rank (descending)
    static std::array<Card, 5> sort_by_rank(const std::array<Card, 5>& cards);
};

} // namespace poker
