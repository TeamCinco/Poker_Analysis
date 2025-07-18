#include "card.h"
#include "platform_utils.h"
#include <algorithm>
#include <random>
#include <chrono>
#include <stdexcept>

namespace poker {
    Card::Card(Rank rank, Suit suit) : rank_(rank), suit_(suit) {}

    std::string Card::toString() const {
        std::string rank_str;
        switch (rank_) {
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
        
        std::string suit_str = platform::getSuitSymbol(static_cast<int>(suit_));
        
        // Add color support if available
        if (platform::supportsColors()) {
            if (suit_ == Suit::HEARTS || suit_ == Suit::DIAMONDS) {
                suit_str = platform::colorize(suit_str, platform::Color::RED);
            } else {
                suit_str = platform::colorize(suit_str, platform::Color::WHITE);
            }
        }
        
        return rank_str + suit_str;
    }

    bool Card::operator==(const Card& other) const {
        return rank_ == other.rank_ && suit_ == other.suit_;
    }

    bool Card::operator<(const Card& other) const {
        if (rank_ != other.rank_) {
            return rank_ < other.rank_;
        }
        return suit_ < other.suit_;
    }

    Deck::Deck() : current_card_(0) {
        reset();
    }

    void Deck::reset() {
        cards_.clear();
        current_card_ = 0;
        
        // Create all 52 cards
        for (int suit = 0; suit < 4; ++suit) {
            for (int rank = 2; rank <= 14; ++rank) {
                cards_.emplace_back(static_cast<Rank>(rank), static_cast<Suit>(suit));
            }
        }
    }

    void Deck::shuffle() {
        auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        std::shuffle(cards_.begin(), cards_.end(), std::default_random_engine(seed));
        current_card_ = 0;
    }

    Card Deck::dealCard() {
        if (current_card_ >= cards_.size()) {
            throw std::runtime_error("No more cards in deck");
        }
        return cards_[current_card_++];
    }

    std::vector<Card> Deck::dealCards(int count) {
        std::vector<Card> dealt_cards;
        for (int i = 0; i < count; ++i) {
            dealt_cards.push_back(dealCard());
        }
        return dealt_cards;
    }
}
