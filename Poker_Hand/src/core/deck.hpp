#pragma once

#include "types.hpp"
#include <array>
#include <random>
#include <algorithm>

namespace poker {

class Deck {
public:
    Deck();
    
    // Initialize a fresh deck
    void initialize();
    
    // Shuffle the deck using Fisher-Yates algorithm
    void shuffle(std::mt19937_64& rng);
    
    // Deal cards from the deck
    Card deal_card();
    std::array<Card, 2> deal_hole_cards();
    std::array<Card, 5> deal_board();
    
    // Remove specific cards from the deck (for known hole cards)
    void remove_cards(const std::array<Card, 2>& cards);
    void remove_card(Card card);
    
    // Reset deck with removed cards
    void reset_with_removed_cards(const std::array<Card, 2>& hero_cards);
    
    // Get remaining cards count
    int remaining_cards() const;
    
    // Check if deck is empty
    bool empty() const;
    
    // Get all remaining cards
    const std::vector<Card>& get_remaining_cards() const;
    
    // SIMD-optimized batch operations
    void deal_multiple_hands(int num_hands, std::vector<std::array<Card, 2>>& hands);
    void deal_multiple_boards(int num_boards, std::vector<std::array<Card, 5>>& boards);

private:
    std::array<Card, NUM_CARDS> full_deck_;
    std::vector<Card> remaining_cards_;
    int current_position_;
    
    // Initialize the standard 52-card deck
    void create_standard_deck();
    
    // Fast shuffle using SIMD when possible
    void simd_shuffle(std::mt19937_64& rng);
    
    // Verify deck integrity
    bool verify_deck_integrity() const;
};

// Thread-local deck for parallel processing
thread_local extern Deck g_thread_deck;

} // namespace poker
