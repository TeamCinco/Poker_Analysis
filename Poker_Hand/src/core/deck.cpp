#include "deck.hpp"
#include <algorithm>
#include <random>
#include <stdexcept>

namespace poker {

// Thread-local deck instance
thread_local Deck g_thread_deck;

Deck::Deck() : current_position_(0) {
    create_standard_deck();
    initialize();
}

void Deck::initialize() {
    remaining_cards_.clear();
    remaining_cards_.reserve(NUM_CARDS);
    
    for (int i = 0; i < NUM_CARDS; ++i) {
        remaining_cards_.push_back(full_deck_[i]);
    }
    
    current_position_ = 0;
}

void Deck::create_standard_deck() {
    int card_index = 0;
    
    // Create all 52 cards: 13 ranks × 4 suits
    for (int suit = 0; suit < 4; ++suit) {
        for (int rank = 2; rank <= 14; ++rank) {  // 2 through Ace
            full_deck_[card_index++] = make_card(rank, suit);
        }
    }
}

void Deck::shuffle(std::mt19937_64& rng) {
    if (remaining_cards_.size() > 1) {
        // Fisher-Yates shuffle
        for (int i = remaining_cards_.size() - 1; i > 0; --i) {
            std::uniform_int_distribution<int> dist(0, i);
            int j = dist(rng);
            std::swap(remaining_cards_[i], remaining_cards_[j]);
        }
    }
    current_position_ = 0;
}

void Deck::simd_shuffle(std::mt19937_64& rng) {
    // For now, fall back to standard shuffle
    // In production, this would use SIMD instructions for faster shuffling
    shuffle(rng);
}

Card Deck::deal_card() {
    if (current_position_ >= remaining_cards_.size()) {
        throw std::runtime_error("Cannot deal card: deck is empty");
    }
    
    return remaining_cards_[current_position_++];
}

std::array<Card, 2> Deck::deal_hole_cards() {
    if (current_position_ + 2 > remaining_cards_.size()) {
        throw std::runtime_error("Cannot deal hole cards: insufficient cards");
    }
    
    std::array<Card, 2> hole_cards = {
        remaining_cards_[current_position_++],
        remaining_cards_[current_position_++]
    };
    
    return hole_cards;
}

std::array<Card, 5> Deck::deal_board() {
    if (current_position_ + 5 > remaining_cards_.size()) {
        throw std::runtime_error("Cannot deal board: insufficient cards");
    }
    
    std::array<Card, 5> board = {
        remaining_cards_[current_position_++],
        remaining_cards_[current_position_++],
        remaining_cards_[current_position_++],
        remaining_cards_[current_position_++],
        remaining_cards_[current_position_++]
    };
    
    return board;
}

void Deck::remove_cards(const std::array<Card, 2>& cards) {
    for (const auto& card : cards) {
        remove_card(card);
    }
}

void Deck::remove_card(Card card) {
    auto it = std::find(remaining_cards_.begin(), remaining_cards_.end(), card);
    if (it != remaining_cards_.end()) {
        remaining_cards_.erase(it);
    }
}

void Deck::reset_with_removed_cards(const std::array<Card, 2>& hero_cards) {
    initialize();
    remove_cards(hero_cards);
    current_position_ = 0;
}

int Deck::remaining_cards() const {
    return remaining_cards_.size() - current_position_;
}

bool Deck::empty() const {
    return current_position_ >= remaining_cards_.size();
}

const std::vector<Card>& Deck::get_remaining_cards() const {
    return remaining_cards_;
}

void Deck::deal_multiple_hands(int num_hands, std::vector<std::array<Card, 2>>& hands) {
    hands.clear();
    hands.reserve(num_hands);
    
    for (int i = 0; i < num_hands; ++i) {
        if (current_position_ + 2 <= remaining_cards_.size()) {
            hands.push_back(deal_hole_cards());
        } else {
            break;  // Not enough cards
        }
    }
}

void Deck::deal_multiple_boards(int num_boards, std::vector<std::array<Card, 5>>& boards) {
    boards.clear();
    boards.reserve(num_boards);
    
    for (int i = 0; i < num_boards; ++i) {
        if (current_position_ + 5 <= remaining_cards_.size()) {
            boards.push_back(deal_board());
        } else {
            break;  // Not enough cards
        }
    }
}

bool Deck::verify_deck_integrity() const {
    // Check that all cards are unique
    std::array<bool, NUM_CARDS> card_present = {false};
    
    for (const auto& card : remaining_cards_) {
        int rank = get_rank(card);
        int suit = get_suit(card);
        
        if (rank < 2 || rank > 14 || suit < 0 || suit > 3) {
            return false;  // Invalid card
        }
        
        int card_index = (rank - 2) * 4 + suit;
        if (card_present[card_index]) {
            return false;  // Duplicate card
        }
        card_present[card_index] = true;
    }
    
    return true;
}

} // namespace poker
