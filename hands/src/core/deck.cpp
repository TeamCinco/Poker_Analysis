#include "deck.hpp"
#include <algorithm>
#include <stdexcept>

namespace poker {

Deck::Deck() : current_index(0) {
    reset();
}

void Deck::reset() {
    cards.clear();
    current_index = 0;
    
    // Create all 52 cards
    for (int suit = 0; suit < 4; ++suit) {
        for (int rank = 2; rank <= 14; ++rank) {
            cards.emplace_back(static_cast<Rank>(rank), static_cast<Suit>(suit));
        }
    }
}

void Deck::shuffle(std::mt19937& rng) {
    current_index = 0;
    std::shuffle(cards.begin(), cards.end(), rng);
}

Card Deck::deal() {
    if (empty()) {
        throw std::runtime_error("Cannot deal from empty deck");
    }
    return cards[current_index++];
}

void Deck::remove_cards(const std::vector<Card>& cards_to_remove) {
    for (const auto& card_to_remove : cards_to_remove) {
        auto it = std::find(cards.begin(), cards.end(), card_to_remove);
        if (it != cards.end()) {
            cards.erase(it);
        }
    }
    current_index = 0;
}

std::array<Card, 2> Deck::parse_hand(const std::string& hand_str) {
    if (hand_str.length() != 4) {
        throw std::invalid_argument("Hand string must be 4 characters (e.g., 'AhKd')");
    }
    
    Card card1 = parse_card(hand_str.substr(0, 2));
    Card card2 = parse_card(hand_str.substr(2, 2));
    
    return {card1, card2};
}

std::vector<Card> Deck::parse_board(const std::string& board_str) {
    if (board_str.empty()) {
        return {};
    }
    
    if (board_str.length() % 2 != 0) {
        throw std::invalid_argument("Board string must have even number of characters");
    }
    
    std::vector<Card> board;
    for (size_t i = 0; i < board_str.length(); i += 2) {
        board.push_back(parse_card(board_str.substr(i, 2)));
    }
    
    return board;
}

Card Deck::parse_card(const std::string& card_str) {
    if (card_str.length() != 2) {
        throw std::invalid_argument("Card string must be 2 characters (e.g., 'Ah')");
    }
    
    Rank rank = char_to_rank(card_str[0]);
    Suit suit = char_to_suit(card_str[1]);
    
    return Card(rank, suit);
}

Rank Deck::char_to_rank(char c) {
    switch (c) {
        case '2': return Rank::TWO;
        case '3': return Rank::THREE;
        case '4': return Rank::FOUR;
        case '5': return Rank::FIVE;
        case '6': return Rank::SIX;
        case '7': return Rank::SEVEN;
        case '8': return Rank::EIGHT;
        case '9': return Rank::NINE;
        case 'T': case 't': return Rank::TEN;
        case 'J': case 'j': return Rank::JACK;
        case 'Q': case 'q': return Rank::QUEEN;
        case 'K': case 'k': return Rank::KING;
        case 'A': case 'a': return Rank::ACE;
        default:
            throw std::invalid_argument("Invalid rank character: " + std::string(1, c));
    }
}

Suit Deck::char_to_suit(char c) {
    switch (c) {
        case 'c': case 'C': return Suit::CLUBS;
        case 'd': case 'D': return Suit::DIAMONDS;
        case 'h': case 'H': return Suit::HEARTS;
        case 's': case 'S': return Suit::SPADES;
        default:
            throw std::invalid_argument("Invalid suit character: " + std::string(1, c));
    }
}

} // namespace poker
