#include "simulator.h"
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <map>
#include <string>
#include <numeric>

namespace poker {
    PokerSimulator::PokerSimulator() : card_dist_(0, 51) {
        initializeRNG();
    }

    void PokerSimulator::initializeRNG() {
        auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        rng_.seed(static_cast<unsigned int>(seed));
    }

    std::map<HandType, int> PokerSimulator::runHandSimulation(int num_hands) {
        std::map<HandType, int> hand_counts;
        
        // Initialize counts
        for (int i = 1; i <= 10; ++i) {
            hand_counts[static_cast<HandType>(i)] = 0;
        }
        
        for (int i = 0; i < num_hands; ++i) {
            Deck deck;
            deck.shuffle();
            
            std::vector<Card> cards = deck.dealCards(5);
            Hand hand(cards);
            HandResult result = hand.evaluate();
            
            hand_counts[result.type]++;
        }
        
        return hand_counts;
    }

    double PokerSimulator::simulateHoldemWinRate(const std::vector<Card>& hole_cards, 
                                               int num_opponents, 
                                               int num_simulations) {
        if (hole_cards.size() != 2) {
            throw std::invalid_argument("Texas Hold'em requires exactly 2 hole cards");
        }
        
        int wins = 0;
        
        for (int sim = 0; sim < num_simulations; ++sim) {
            // Create deck without the hole cards
            Deck deck;
            deck.reset();
            
            // Remove hole cards from deck (simulate dealing them)
            std::vector<Card> available_cards;
            for (int suit = 0; suit < 4; ++suit) {
                for (int rank = 2; rank <= 14; ++rank) {
                    Card card(static_cast<Rank>(rank), static_cast<Suit>(suit));
                    bool is_hole_card = false;
                    for (const auto& hole_card : hole_cards) {
                        if (card == hole_card) {
                            is_hole_card = true;
                            break;
                        }
                    }
                    if (!is_hole_card) {
                        available_cards.push_back(card);
                    }
                }
            }
            
            // Shuffle available cards
            std::shuffle(available_cards.begin(), available_cards.end(), rng_);
            
            // Deal community cards
            std::vector<Card> community_cards;
            for (int i = 0; i < 5; ++i) {
                community_cards.push_back(available_cards[i]);
            }
            
            // Evaluate player's hand
            Hand player_hand = evaluateBestHand(hole_cards, community_cards);
            HandResult player_result = player_hand.evaluate();
            
            // Simulate opponent hands
            bool player_wins = true;
            int card_index = 5; // Start after community cards
            
            for (int opp = 0; opp < num_opponents; ++opp) {
                if (card_index + 1 >= available_cards.size()) {
                    break; // Not enough cards for all opponents
                }
                
                std::vector<Card> opponent_hole_cards = {
                    available_cards[card_index],
                    available_cards[card_index + 1]
                };
                card_index += 2;
                
                Hand opponent_hand = evaluateBestHand(opponent_hole_cards, community_cards);
                HandResult opponent_result = opponent_hand.evaluate();
                
                if (opponent_result > player_result) {
                    player_wins = false;
                    break;
                }
            }
            
            if (player_wins) {
                wins++;
            }
        }
        
        return static_cast<double>(wins) / num_simulations;
    }

    PokerSimulator::TournamentResult PokerSimulator::simulateTournament(const std::vector<std::string>& players, 
                                                                       int num_tournaments) {
        TournamentResult result;
        
        // Initialize results
        for (const auto& player : players) {
            result.finishing_positions[player] = 0;
            result.roi[player] = 0.0;
        }
        
        for (int tournament = 0; tournament < num_tournaments; ++tournament) {
            // Simulate a simple tournament (random elimination for now)
            std::vector<std::string> remaining_players = players;
            std::shuffle(remaining_players.begin(), remaining_players.end(), rng_);
            
            // Assign finishing positions (reverse order of elimination)
            for (int position = players.size(); position >= 1; --position) {
                std::string eliminated_player = remaining_players[position - 1];
                result.finishing_positions[eliminated_player] += position;
            }
        }
        
        // Calculate average finishing positions and ROI
        for (const auto& player : players) {
            result.finishing_positions[player] /= num_tournaments;
            
            // Simple ROI calculation based on finishing position
            double avg_position = static_cast<double>(result.finishing_positions[player]);
            double total_players = static_cast<double>(players.size());
            result.roi[player] = (total_players - avg_position) / total_players;
        }
        
        return result;
    }

    std::vector<Card> PokerSimulator::generateCommunityCards() {
        Deck deck;
        deck.shuffle();
        return deck.dealCards(5);
    }

    Hand PokerSimulator::evaluateBestHand(const std::vector<Card>& hole_cards, 
                                        const std::vector<Card>& community_cards) {
        std::vector<Card> all_cards = hole_cards;
        all_cards.insert(all_cards.end(), community_cards.begin(), community_cards.end());
        
        if (all_cards.size() != 7) {
            throw std::invalid_argument("Need exactly 7 cards to evaluate best hand");
        }
        
        // Hand constructor will automatically find the best 5-card combination
        return Hand(all_cards);
    }
}
