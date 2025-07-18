#include "types.hpp"
#include <unordered_map>
#include <algorithm>
#include <cctype>
#include <stdexcept>

namespace poker {

std::pair<Card, Card> parse_hand_string(const std::string& hand_str) {
    if (hand_str.length() < 2 || hand_str.length() > 3) {
        throw std::invalid_argument("Invalid hand string format");
    }
    
    static const std::unordered_map<char, int> rank_map = {
        {'2', 2}, {'3', 3}, {'4', 4}, {'5', 5}, {'6', 6}, {'7', 7}, {'8', 8},
        {'9', 9}, {'T', 10}, {'J', 11}, {'Q', 12}, {'K', 13}, {'A', 14}
    };
    
    char rank1 = std::toupper(hand_str[0]);
    char rank2 = std::toupper(hand_str[1]);
    
    auto it1 = rank_map.find(rank1);
    auto it2 = rank_map.find(rank2);
    
    if (it1 == rank_map.end() || it2 == rank_map.end()) {
        throw std::invalid_argument("Invalid card rank");
    }
    
    int r1 = it1->second;
    int r2 = it2->second;
    
    // Determine if suited or offsuit
    bool is_suited = (hand_str.length() == 3 && std::tolower(hand_str[2]) == 's');
    bool is_pair = (r1 == r2);
    
    Card card1, card2;
    
    if (is_pair) {
        // For pairs, use different suits
        card1 = make_card(r1, 0);  // clubs
        card2 = make_card(r2, 1);  // diamonds
    } else {
        // Ensure higher rank is first
        if (r1 < r2) {
            std::swap(r1, r2);
        }
        
        if (is_suited) {
            card1 = make_card(r1, 0);  // clubs
            card2 = make_card(r2, 0);  // clubs
        } else {
            card1 = make_card(r1, 0);  // clubs
            card2 = make_card(r2, 1);  // diamonds
        }
    }
    
    return {card1, card2};
}

std::string hand_to_string(Card card1, Card card2) {
    int rank1 = get_rank(card1);
    int rank2 = get_rank(card2);
    int suit1 = get_suit(card1);
    int suit2 = get_suit(card2);
    
    const char* ranks = "23456789TJQKA";
    
    // Ensure higher rank is first for non-pairs
    if (rank1 < rank2) {
        std::swap(rank1, rank2);
        std::swap(suit1, suit2);
    }
    
    std::string result;
    result += ranks[rank1 - 2];
    result += ranks[rank2 - 2];
    
    if (rank1 != rank2) {  // Not a pair
        if (suit1 == suit2) {
            result += 's';  // suited
        } else {
            result += 'o';  // offsuit
        }
    }
    
    return result;
}

} // namespace poker
