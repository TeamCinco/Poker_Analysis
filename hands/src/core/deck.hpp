#pragma once

#include "types.hpp"
#include <vector>
#include <random>
#include <array>

namespace poker {

class Deck {
public:
    Deck();
    
    // Shuffle the deck
    void shuffle(std::mt19937& rng);
    
    // Deal a card (removes from deck)
    Card deal();
    
    // Reset deck to full 52 cards
    void reset();
    
    // Remove specific cards from deck (for dealing hole cards/board)
    void remove_cards(const std::vector<Card>& cards);
    
    // Check if deck has cards
    bool empty() const { return current_index >= cards.size(); }
    
    // Get remaining cards count
    size_t remaining_cards() const { return cards.size() - current_index; }
    
    // Parse hand string like "AhKd" into cards
    static std::array<Card, 2> parse_hand(const std::string& hand_str);
    
    // Parse board string like "As7h2c" into cards
    static std::vector<Card> parse_board(const std::string& board_str);

private:
    std::vector<Card> cards;
    size_t current_index;
    
    // Helper to parse single card string like "Ah"
    static Card parse_card(const std::string& card_str);
    static Rank char_to_rank(char c);
    static Suit char_to_suit(char c);
};

} // namespace poker
