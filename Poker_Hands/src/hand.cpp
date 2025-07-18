#include "hand.h"
#include <algorithm>
#include <map>
#include <set>
#include <numeric>
#include <stdexcept>

namespace poker {
    Hand::Hand(const std::vector<Card>& cards) : cards_(cards) {
        if (cards.size() > 7) {
            throw std::invalid_argument("Hand cannot have more than 7 cards");
        }
    }

    HandResult Hand::evaluate() const {
        if (cards_.size() < 5) {
            throw std::invalid_argument("Need at least 5 cards to evaluate hand");
        }
        
        // If more than 5 cards, find best 5-card combination
        if (cards_.size() > 5) {
            HandResult best_result;
            best_result.type = HandType::HIGH_CARD;
            
            // Generate all 5-card combinations
            std::vector<int> indices(cards_.size());
            std::iota(indices.begin(), indices.end(), 0);
            
            std::vector<bool> selector(cards_.size(), false);
            std::fill(selector.begin(), selector.begin() + 5, true);
            
            do {
                std::vector<Card> five_cards;
                for (int i = 0; i < cards_.size(); ++i) {
                    if (selector[i]) {
                        five_cards.push_back(cards_[i]);
                    }
                }
                
                Hand temp_hand(five_cards);
                HandResult result = temp_hand.evaluateHand();
                
                if (result > best_result) {
                    best_result = result;
                }
            } while (std::prev_permutation(selector.begin(), selector.end()));
            
            return best_result;
        }
        
        return evaluateHand();
    }

    HandResult Hand::evaluateHand() const {
        HandResult result;
        
        // Sort cards by rank (descending)
        std::vector<Card> sorted_cards = cards_;
        std::sort(sorted_cards.begin(), sorted_cards.end(), 
                  [](const Card& a, const Card& b) {
                      return a.getRank() > b.getRank();
                  });
        
        bool flush = isFlush();
        bool straight = isStraight();
        std::vector<int> rank_counts = getRankCounts();
        
        // Check for royal flush
        if (flush && straight && sorted_cards[0].getRank() == Rank::ACE) {
            result.type = HandType::ROYAL_FLUSH;
            result.strength = 10;
            return result;
        }
        
        // Check for straight flush
        if (flush && straight) {
            result.type = HandType::STRAIGHT_FLUSH;
            result.strength = 9;
            result.kickers.push_back(sorted_cards[0].getRank());
            return result;
        }
        
        // Check for four of a kind
        if (rank_counts[0] == 4) {
            result.type = HandType::FOUR_OF_A_KIND;
            result.strength = 8;
            return result;
        }
        
        // Check for full house
        if (rank_counts[0] == 3 && rank_counts[1] == 2) {
            result.type = HandType::FULL_HOUSE;
            result.strength = 7;
            return result;
        }
        
        // Check for flush
        if (flush) {
            result.type = HandType::FLUSH;
            result.strength = 6;
            for (const auto& card : sorted_cards) {
                result.kickers.push_back(card.getRank());
            }
            return result;
        }
        
        // Check for straight
        if (straight) {
            result.type = HandType::STRAIGHT;
            result.strength = 5;
            result.kickers.push_back(sorted_cards[0].getRank());
            return result;
        }
        
        // Check for three of a kind
        if (rank_counts[0] == 3) {
            result.type = HandType::THREE_OF_A_KIND;
            result.strength = 4;
            return result;
        }
        
        // Check for two pair
        if (rank_counts[0] == 2 && rank_counts[1] == 2) {
            result.type = HandType::TWO_PAIR;
            result.strength = 3;
            return result;
        }
        
        // Check for one pair
        if (rank_counts[0] == 2) {
            result.type = HandType::ONE_PAIR;
            result.strength = 2;
            return result;
        }
        
        // High card
        result.type = HandType::HIGH_CARD;
        result.strength = 1;
        for (const auto& card : sorted_cards) {
            result.kickers.push_back(card.getRank());
        }
        
        return result;
    }

    bool Hand::isFlush() const {
        if (cards_.size() < 5) return false;
        
        Suit first_suit = cards_[0].getSuit();
        for (size_t i = 1; i < cards_.size(); ++i) {
            if (cards_[i].getSuit() != first_suit) {
                return false;
            }
        }
        return true;
    }

    bool Hand::isStraight() const {
        if (cards_.size() < 5) return false;
        
        std::vector<int> ranks;
        for (const auto& card : cards_) {
            ranks.push_back(static_cast<int>(card.getRank()));
        }
        
        std::sort(ranks.begin(), ranks.end());
        
        // Check for regular straight
        for (size_t i = 1; i < ranks.size(); ++i) {
            if (ranks[i] != ranks[i-1] + 1) {
                // Check for wheel (A-2-3-4-5)
                if (ranks[0] == 2 && ranks[1] == 3 && ranks[2] == 4 && 
                    ranks[3] == 5 && ranks[4] == 14) {
                    return true;
                }
                return false;
            }
        }
        
        return true;
    }

    std::vector<int> Hand::getRankCounts() const {
        std::map<Rank, int> rank_count;
        for (const auto& card : cards_) {
            rank_count[card.getRank()]++;
        }
        
        std::vector<int> counts;
        for (const auto& pair : rank_count) {
            counts.push_back(pair.second);
        }
        
        std::sort(counts.begin(), counts.end(), std::greater<int>());
        return counts;
    }

    std::string Hand::handTypeToString(HandType type) {
        switch (type) {
            case HandType::HIGH_CARD: return "High Card";
            case HandType::ONE_PAIR: return "One Pair";
            case HandType::TWO_PAIR: return "Two Pair";
            case HandType::THREE_OF_A_KIND: return "Three of a Kind";
            case HandType::STRAIGHT: return "Straight";
            case HandType::FLUSH: return "Flush";
            case HandType::FULL_HOUSE: return "Full House";
            case HandType::FOUR_OF_A_KIND: return "Four of a Kind";
            case HandType::STRAIGHT_FLUSH: return "Straight Flush";
            case HandType::ROYAL_FLUSH: return "Royal Flush";
            default: return "Unknown";
        }
    }

    bool HandResult::operator>(const HandResult& other) const {
        if (strength != other.strength) {
            return strength > other.strength;
        }
        
        // Compare kickers
        for (size_t i = 0; i < std::min(kickers.size(), other.kickers.size()); ++i) {
            if (kickers[i] != other.kickers[i]) {
                return kickers[i] > other.kickers[i];
            }
        }
        
        return false;
    }

    std::string HandResult::toString() const {
        return Hand::handTypeToString(type);
    }

    void Hand::addCard(const Card& card) {
        if (cards_.size() >= 7) {
            throw std::runtime_error("Cannot add more than 7 cards to hand");
        }
        cards_.push_back(card);
    }

    void Hand::clear() {
        cards_.clear();
    }
}
