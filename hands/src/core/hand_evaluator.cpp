#include "hand_evaluator.hpp"
#include <algorithm>
#include <map>
#include <set>

namespace poker {

HandValue HandEvaluator::evaluate_hand(const std::array<Card, 5>& cards) {
    auto sorted_cards = sort_by_rank(cards);
    
    bool flush = is_flush(sorted_cards);
    bool straight = is_straight(sorted_cards);
    auto rank_counts = get_rank_counts(sorted_cards);
    
    HandValue result;
    
    // Check for straight flush / royal flush
    if (flush && straight) {
        if (static_cast<int>(sorted_cards[0].rank) == 14) { // Ace high straight
            result.rank = HandRank::ROYAL_FLUSH;
        } else {
            result.rank = HandRank::STRAIGHT_FLUSH;
        }
    }
    // Check for four of a kind
    else if (rank_counts[0] == 4) {
        result.rank = HandRank::FOUR_OF_A_KIND;
    }
    // Check for full house
    else if (rank_counts[0] == 3 && rank_counts[1] == 2) {
        result.rank = HandRank::FULL_HOUSE;
    }
    // Check for flush
    else if (flush) {
        result.rank = HandRank::FLUSH;
    }
    // Check for straight
    else if (straight) {
        result.rank = HandRank::STRAIGHT;
    }
    // Check for three of a kind
    else if (rank_counts[0] == 3) {
        result.rank = HandRank::THREE_OF_A_KIND;
    }
    // Check for two pair
    else if (rank_counts[0] == 2 && rank_counts[1] == 2) {
        result.rank = HandRank::TWO_PAIR;
    }
    // Check for pair
    else if (rank_counts[0] == 2) {
        result.rank = HandRank::PAIR;
    }
    // High card
    else {
        result.rank = HandRank::HIGH_CARD;
    }
    
    result.strength = calculate_strength(result.rank, sorted_cards);
    return result;
}

HandValue HandEvaluator::evaluate_best_hand(const std::array<Card, 2>& hole_cards, 
                                           const std::vector<Card>& board) {
    if (board.size() < 3) {
        // Can't make a 5-card hand yet
        HandValue result;
        result.rank = HandRank::HIGH_CARD;
        result.strength = 0;
        return result;
    }
    
    auto combinations = get_all_combinations(hole_cards, board);
    
    HandValue best_hand;
    best_hand.strength = -1;
    
    for (const auto& combination : combinations) {
        HandValue hand_value = evaluate_hand(combination);
        if (hand_value.strength > best_hand.strength) {
            best_hand = hand_value;
        }
    }
    
    return best_hand;
}

int HandEvaluator::compare_hands(const HandValue& hand1, const HandValue& hand2) {
    if (hand1.strength > hand2.strength) return 1;
    if (hand1.strength < hand2.strength) return -1;
    return 0;
}

bool HandEvaluator::is_flush(const std::array<Card, 5>& cards) {
    Suit suit = cards[0].suit;
    for (int i = 1; i < 5; ++i) {
        if (cards[i].suit != suit) {
            return false;
        }
    }
    return true;
}

bool HandEvaluator::is_straight(const std::array<Card, 5>& cards) {
    auto sorted = sort_by_rank(cards);
    
    // Check for A-2-3-4-5 straight (wheel)
    if (static_cast<int>(sorted[0].rank) == 14 && 
        static_cast<int>(sorted[1].rank) == 5 &&
        static_cast<int>(sorted[2].rank) == 4 &&
        static_cast<int>(sorted[3].rank) == 3 &&
        static_cast<int>(sorted[4].rank) == 2) {
        return true;
    }
    
    // Check for normal straight
    for (int i = 1; i < 5; ++i) {
        if (static_cast<int>(sorted[i-1].rank) - static_cast<int>(sorted[i].rank) != 1) {
            return false;
        }
    }
    return true;
}

std::vector<int> HandEvaluator::get_rank_counts(const std::array<Card, 5>& cards) {
    std::map<Rank, int> counts;
    for (const auto& card : cards) {
        counts[card.rank]++;
    }
    
    std::vector<int> count_values;
    for (const auto& pair : counts) {
        count_values.push_back(pair.second);
    }
    
    std::sort(count_values.rbegin(), count_values.rend());
    return count_values;
}

int HandEvaluator::calculate_strength(HandRank rank, const std::array<Card, 5>& cards) {
    int base_strength = static_cast<int>(rank) * 1000000;
    
    auto sorted = sort_by_rank(cards);
    
    // Add high card values for tie-breaking
    for (int i = 0; i < 5; ++i) {
        base_strength += static_cast<int>(sorted[i].rank) * (100000 / (i + 1));
    }
    
    return base_strength;
}

std::vector<std::array<Card, 5>> HandEvaluator::get_all_combinations(
    const std::array<Card, 2>& hole_cards, const std::vector<Card>& board) {
    
    std::vector<Card> all_cards;
    all_cards.push_back(hole_cards[0]);
    all_cards.push_back(hole_cards[1]);
    for (const auto& card : board) {
        all_cards.push_back(card);
    }
    
    std::vector<std::array<Card, 5>> combinations;
    
    // Generate all 5-card combinations from 7 cards
    if (all_cards.size() == 7) {
        for (int i = 0; i < 7; ++i) {
            for (int j = i + 1; j < 7; ++j) {
                for (int k = j + 1; k < 7; ++k) {
                    for (int l = k + 1; l < 7; ++l) {
                        for (int m = l + 1; m < 7; ++m) {
                            std::array<Card, 5> combination = {
                                all_cards[i], all_cards[j], all_cards[k], 
                                all_cards[l], all_cards[m]
                            };
                            combinations.push_back(combination);
                        }
                    }
                }
            }
        }
    }
    // For 5 or 6 cards, just use all available
    else if (all_cards.size() >= 5) {
        std::array<Card, 5> combination;
        for (int i = 0; i < 5; ++i) {
            combination[i] = all_cards[i];
        }
        combinations.push_back(combination);
    }
    
    return combinations;
}

std::array<Card, 5> HandEvaluator::sort_by_rank(const std::array<Card, 5>& cards) {
    auto sorted = cards;
    std::sort(sorted.begin(), sorted.end(), [](const Card& a, const Card& b) {
        return static_cast<int>(a.rank) > static_cast<int>(b.rank);
    });
    return sorted;
}

} // namespace poker
