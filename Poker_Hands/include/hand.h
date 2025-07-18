#pragma once
#include "card.h"
#include <vector>
#include <string>

namespace poker {
    enum class HandType {
        HIGH_CARD = 1,
        ONE_PAIR = 2,
        TWO_PAIR = 3,
        THREE_OF_A_KIND = 4,
        STRAIGHT = 5,
        FLUSH = 6,
        FULL_HOUSE = 7,
        FOUR_OF_A_KIND = 8,
        STRAIGHT_FLUSH = 9,
        ROYAL_FLUSH = 10
    };

    struct HandResult {
        HandType type;
        std::vector<Rank> kickers;
        int strength;
        
        bool operator>(const HandResult& other) const;
        std::string toString() const;
    };

    class Hand {
    public:
        Hand(const std::vector<Card>& cards);
        
        HandResult evaluate() const;
        std::vector<Card> getCards() const { return cards_; }
        void addCard(const Card& card);
        void clear();
        size_t size() const { return cards_.size(); }
        
        static std::string handTypeToString(HandType type);
        
    private:
        std::vector<Card> cards_;
        
        bool isFlush() const;
        bool isStraight() const;
        std::vector<int> getRankCounts() const;
        HandResult evaluateHand() const;
    };
}