#include "analyzer.h"
#include "platform_utils.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <stdexcept>

namespace poker {
    PokerAnalyzer::PokerAnalyzer() {}

    std::map<HandType, HandStatistics> PokerAnalyzer::calculateExactProbabilities() {
        std::cout << "Calculating exact probabilities for 5-card poker hands...\n";
        
        std::map<HandType, int> hand_counts;
        int total_hands = 0;
        
        // Initialize counts
        for (int i = 1; i <= 10; ++i) {
            hand_counts[static_cast<HandType>(i)] = 0;
        }
        
        // Generate all combinations of 5 cards from 52
        Deck deck;
        std::vector<Card> all_cards;
        
        // Get all cards from deck
        for (int suit = 0; suit < 4; ++suit) {
            for (int rank = 2; rank <= 14; ++rank) {
                all_cards.emplace_back(static_cast<Rank>(rank), static_cast<Suit>(suit));
            }
        }
        
        // Generate all 5-card combinations (C(52,5) = 2,598,960)
        std::vector<bool> selector(52, false);
        std::fill(selector.begin(), selector.begin() + 5, true);
        
        do {
            std::vector<Card> hand;
            for (int i = 0; i < 52; ++i) {
                if (selector[i]) {
                    hand.push_back(all_cards[i]);
                }
            }
            
            Hand poker_hand(hand);
            HandResult result = poker_hand.evaluate();
            hand_counts[result.type]++;
            total_hands++;
            
            if (total_hands % 100000 == 0) {
                std::cout << "Processed " << total_hands << " hands...\n";
            }
            
        } while (std::prev_permutation(selector.begin(), selector.end()));
        
        // Calculate statistics
        std::map<HandType, HandStatistics> stats;
        for (const auto& pair : hand_counts) {
            HandStatistics stat;
            stat.count = pair.second;
            stat.probability = static_cast<double>(pair.second) / total_hands;
            stat.percentage = stat.probability * 100.0;
            
            if (pair.second > 0) {
                stat.odds = "1 in " + std::to_string(total_hands / pair.second);
            } else {
                stat.odds = "Impossible";
            }
            
            stats[pair.first] = stat;
        }
        
        return stats;
    }

    std::map<HandType, HandStatistics> PokerAnalyzer::simulateHands(int num_simulations) {
        std::cout << "Running " << num_simulations << " hand simulations...\n";
        
        std::map<HandType, int> hand_counts;
        
        // Initialize counts
        for (int i = 1; i <= 10; ++i) {
            hand_counts[static_cast<HandType>(i)] = 0;
        }
        
        for (int i = 0; i < num_simulations; ++i) {
            Deck deck;
            deck.shuffle();
            
            std::vector<Card> cards = deck.dealCards(5);
            Hand hand(cards);
            HandResult result = hand.evaluate();
            
            hand_counts[result.type]++;
            
            if ((i + 1) % 10000 == 0) {
                std::cout << "Simulated " << (i + 1) << " hands...\n";
            }
        }
        
        // Calculate statistics
        std::map<HandType, HandStatistics> stats;
        for (const auto& pair : hand_counts) {
            HandStatistics stat;
            stat.count = pair.second;
            stat.probability = static_cast<double>(pair.second) / num_simulations;
            stat.percentage = stat.probability * 100.0;
            
            if (pair.second > 0) {
                stat.odds = "1 in " + std::to_string(num_simulations / pair.second);
            } else {
                stat.odds = "Never occurred";
            }
            
            stats[pair.first] = stat;
        }
        
        return stats;
    }

    void PokerAnalyzer::displayHandRankings() {
        std::cout << "\n=== POKER HAND RANKINGS ===\n";
        std::cout << std::left << std::setw(20) << "Hand Type" 
                  << std::setw(15) << "Description" << "\n";
        std::cout << std::string(50, '-') << "\n";
        
        std::cout << std::setw(20) << "Royal Flush" 
                  << "A-K-Q-J-T, all same suit\n";
        std::cout << std::setw(20) << "Straight Flush" 
                  << "Five consecutive cards, same suit\n";
        std::cout << std::setw(20) << "Four of a Kind" 
                  << "Four cards of same rank\n";
        std::cout << std::setw(20) << "Full House" 
                  << "Three of a kind + pair\n";
        std::cout << std::setw(20) << "Flush" 
                  << "Five cards of same suit\n";
        std::cout << std::setw(20) << "Straight" 
                  << "Five consecutive cards\n";
        std::cout << std::setw(20) << "Three of a Kind" 
                  << "Three cards of same rank\n";
        std::cout << std::setw(20) << "Two Pair" 
                  << "Two pairs of cards\n";
        std::cout << std::setw(20) << "One Pair" 
                  << "Two cards of same rank\n";
        std::cout << std::setw(20) << "High Card" 
                  << "Highest card when no other hand\n";
    }

    void PokerAnalyzer::displayProbabilities(const std::map<HandType, HandStatistics>& stats) {
        std::cout << "\n" << platform::colorize("=== POKER HAND PROBABILITIES ===", platform::Color::CYAN) << "\n";
        std::cout << std::left << std::setw(20) << platform::bold("Hand Type") 
                  << std::setw(12) << platform::bold("Count") 
                  << std::setw(12) << platform::bold("Probability") 
                  << std::setw(12) << platform::bold("Percentage") 
                  << std::setw(15) << platform::bold("Odds") << "\n";
        std::cout << std::string(75, '-') << "\n";
        
        // Sort by hand strength (descending)
        std::vector<std::pair<HandType, HandStatistics>> sorted_stats;
        for (const auto& pair : stats) {
            sorted_stats.push_back(pair);
        }
        
        std::sort(sorted_stats.begin(), sorted_stats.end(), 
                  [](const auto& a, const auto& b) {
                      return static_cast<int>(a.first) > static_cast<int>(b.first);
                  });
        
        for (const auto& pair : sorted_stats) {
            std::string hand_name = Hand::handTypeToString(pair.first);
            if (pair.first == HandType::ROYAL_FLUSH || pair.first == HandType::STRAIGHT_FLUSH) {
                hand_name = platform::colorize(hand_name, platform::Color::YELLOW);
            }
            
            std::cout << std::setw(20) << hand_name
                      << std::setw(12) << pair.second.count
                      << std::setw(12) << std::fixed << std::setprecision(6) << pair.second.probability
                      << std::setw(12) << std::fixed << std::setprecision(3) << pair.second.percentage << "%"
                      << std::setw(15) << pair.second.odds << "\n";
        }
    }

    std::map<std::string, double> PokerAnalyzer::analyzePreFlopStrength(int num_simulations) {
        std::cout << "Analyzing pre-flop hand strength with " << num_simulations << " simulations...\n";
        
        std::map<std::string, double> results;
        std::vector<std::pair<Rank, Rank>> starting_hands = getPopularStartingHands();
        
        for (const auto& hand_pair : starting_hands) {
            // Test both suited and offsuit versions
            for (bool suited : {true, false}) {
                std::string hand_name = formatStartingHand(hand_pair.first, hand_pair.second, suited);
                
                int wins = 0;
                for (int sim = 0; sim < num_simulations; ++sim) {
                    Deck deck;
                    deck.shuffle();
                    
                    // Deal hole cards
                    std::vector<Card> hole_cards;
                    if (suited) {
                        hole_cards.push_back(Card(hand_pair.first, Suit::SPADES));
                        hole_cards.push_back(Card(hand_pair.second, Suit::SPADES));
                    } else {
                        hole_cards.push_back(Card(hand_pair.first, Suit::SPADES));
                        hole_cards.push_back(Card(hand_pair.second, Suit::HEARTS));
                    }
                    
                    // Simple win rate calculation (placeholder)
                    // This would need proper Hold'em simulation
                    wins += (sim % 3 == 0) ? 1 : 0; // Simplified for now
                }
                
                results[hand_name] = static_cast<double>(wins) / num_simulations;
            }
        }
        
        return results;
    }

    double PokerAnalyzer::calculateWinProbability(const Hand& hand1, const Hand& hand2, int simulations) {
        int hand1_wins = 0;
        
        for (int sim = 0; sim < simulations; ++sim) {
            HandResult result1 = hand1.evaluate();
            HandResult result2 = hand2.evaluate();
            
            if (result1 > result2) {
                hand1_wins++;
            }
        }
        
        return static_cast<double>(hand1_wins) / simulations;
    }

    void PokerAnalyzer::displayPreFlopAnalysis(const std::map<std::string, double>& results) {
        std::cout << "\n" << platform::colorize("=== PRE-FLOP HAND ANALYSIS ===", platform::Color::CYAN) << "\n";
        std::cout << std::left << std::setw(15) << platform::bold("Hand") 
                  << std::setw(15) << platform::bold("Win Rate") << "\n";
        std::cout << std::string(30, '-') << "\n";
        
        // Sort by win rate (descending)
        std::vector<std::pair<std::string, double>> sorted_results;
        for (const auto& pair : results) {
            sorted_results.push_back(pair);
        }
        
        std::sort(sorted_results.begin(), sorted_results.end(), 
                  [](const auto& a, const auto& b) {
                      return a.second > b.second;
                  });
        
        for (const auto& pair : sorted_results) {
            std::string win_rate = std::to_string(pair.second * 100.0).substr(0, 5) + "%";
            std::cout << std::setw(15) << pair.first
                      << std::setw(15) << win_rate << "\n";
        }
    }

    std::vector<std::pair<Rank, Rank>> PokerAnalyzer::getPopularStartingHands() {
        return {
            {Rank::ACE, Rank::ACE},
            {Rank::KING, Rank::KING},
            {Rank::QUEEN, Rank::QUEEN},
            {Rank::ACE, Rank::KING},
            {Rank::ACE, Rank::QUEEN},
            {Rank::JACK, Rank::JACK},
            {Rank::ACE, Rank::JACK},
            {Rank::KING, Rank::QUEEN},
            {Rank::TEN, Rank::TEN},
            {Rank::ACE, Rank::TEN}
        };
    }

    std::string PokerAnalyzer::formatStartingHand(Rank rank1, Rank rank2, bool suited) {
        std::string rank1_str, rank2_str;
        
        auto rankToString = [](Rank rank) -> std::string {
            switch (rank) {
                case Rank::ACE: return "A";
                case Rank::KING: return "K";
                case Rank::QUEEN: return "Q";
                case Rank::JACK: return "J";
                case Rank::TEN: return "T";
                default: return std::to_string(static_cast<int>(rank));
            }
        };
        
        rank1_str = rankToString(rank1);
        rank2_str = rankToString(rank2);
        
        return rank1_str + rank2_str + (suited ? "s" : "o");
    }

    Hand PokerAnalyzer::getBestFiveCardHand(const std::vector<Card>& cards) {
        if (cards.size() <= 5) {
            return Hand(cards);
        }
        
        // If more than 5 cards, find best 5-card combination
        Hand best_hand(std::vector<Card>(cards.begin(), cards.begin() + 5));
        HandResult best_result = best_hand.evaluate();
        
        // Generate all 5-card combinations
        std::vector<bool> selector(cards.size(), false);
        std::fill(selector.begin(), selector.begin() + 5, true);
        
        do {
            std::vector<Card> five_cards;
            for (size_t i = 0; i < cards.size(); ++i) {
                if (selector[i]) {
                    five_cards.push_back(cards[i]);
                }
            }
            
            Hand temp_hand(five_cards);
            HandResult result = temp_hand.evaluate();
            
            if (result > best_result) {
                best_hand = temp_hand;
                best_result = result;
            }
        } while (std::prev_permutation(selector.begin(), selector.end()));
        
        return best_hand;
    }
}
