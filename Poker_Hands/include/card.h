#pragma once
#include <string>
#include <vector>

namespace poker {
    enum class Suit {
        SPADES = 0,
        HEARTS = 1,
        DIAMONDS = 2,
        CLUBS = 3
    };

    enum class Rank {
        TWO = 2, THREE = 3, FOUR = 4, FIVE = 5, SIX = 6, SEVEN = 7,
        EIGHT = 8, NINE = 9, TEN = 10, JACK = 11, QUEEN = 12, KING = 13, ACE = 14
    };

    class Card {
    public:
        Card(Rank rank, Suit suit);
        
        Rank getRank() const { return rank_; }
        Suit getSuit() const { return suit_; }
        
        std::string toString() const;
        bool operator==(const Card& other) const;
        bool operator<(const Card& other) const;
        
    private:
        Rank rank_;
        Suit suit_;
    };

    class Deck {
    public:
        Deck();
        void shuffle();
        void reset();
        Card dealCard();
        std::vector<Card> dealCards(int count);
        bool isEmpty() const { return cards_.empty(); }
        size_t size() const { return cards_.size(); }
        
    private:
        std::vector<Card> cards_;
        size_t current_card_;
    };
}