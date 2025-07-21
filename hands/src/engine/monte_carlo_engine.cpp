#include "monte_carlo_engine.hpp"
#include <iostream>
#include <algorithm>
#include <chrono>

namespace poker {

const std::vector<double> MonteCarloEngine::TEST_FREQUENCIES = {0.0, 0.25, 0.50, 0.75, 1.0};

MonteCarloEngine::MonteCarloEngine() {
    // Seed with current time
    auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    rng_.seed(static_cast<unsigned long>(seed));
}

SimResult MonteCarloEngine::simulate_scenario(const Scenario& scenario) {
    return find_optimal_frequency(scenario, 100000);
}

double MonteCarloEngine::test_frequency(const Scenario& scenario, double frequency, int num_simulations) {
    if (scenario.street == Street::PREFLOP) {
        return simulate_preflop_scenario(scenario, frequency, num_simulations);
    } else {
        return simulate_postflop_scenario(scenario, frequency, num_simulations);
    }
}

SimResult MonteCarloEngine::find_optimal_frequency(const Scenario& scenario, int num_simulations) {
    SimResult result;
    result.table_size = scenario.table_size;
    result.simulations_run = num_simulations * static_cast<int>(TEST_FREQUENCIES.size());
    
    double best_ev = -999.0;
    double best_frequency = 0.0;
    
    // Test each frequency and find the one with highest EV
    for (double frequency : TEST_FREQUENCIES) {
        double ev = test_frequency(scenario, frequency, num_simulations);
        
        if (ev > best_ev) {
            best_ev = ev;
            best_frequency = frequency;
        }
    }
    
    result.optimal_frequency = best_frequency;
    result.expected_value = best_ev;
    result.confidence_interval = 0.01; // Simplified for now
    
    return result;
}

double MonteCarloEngine::simulate_preflop_scenario(const Scenario& scenario, double frequency, int num_simulations) {
    // Parse hero hand using improved parsing
    std::array<Card, 2> hero_hand = parse_starting_hand(scenario.hand);
    
    double total_ev = 0.0;
    
    for (int i = 0; i < num_simulations; ++i) {
        // Decide whether to take action based on frequency
        std::uniform_real_distribution<double> uniform_dist(0.0, 1.0);
        bool take_action = uniform_dist(rng_) < frequency;
        
        if (take_action) {
            // Simulate taking the action
            std::string main_action = scenario.actions.empty() ? "open" : scenario.actions.back();
            double ev = simulate_single_hand(hero_hand, {}, scenario.num_opponents, main_action, 1.0);
            total_ev += ev;
        } else {
            // Simulate folding (EV = 0 for most preflop situations)
            total_ev += 0.0;
        }
    }
    
    return total_ev / num_simulations;
}

double MonteCarloEngine::simulate_postflop_scenario(const Scenario& scenario, double frequency, int num_simulations) {
    // Parse hero hand using improved parsing
    std::array<Card, 2> hero_hand = parse_starting_hand(scenario.hand);
    
    // Parse board
    std::vector<Card> board;
    try {
        board = Deck::parse_board(scenario.board);
    } catch (const std::exception&) {
        // Use default board for parsing errors
        board = {Card(Rank::ACE, Suit::SPADES), Card(Rank::SEVEN, Suit::HEARTS), Card(Rank::TWO, Suit::CLUBS)};
    }
    
    double total_ev = 0.0;
    
    for (int i = 0; i < num_simulations; ++i) {
        std::uniform_real_distribution<double> uniform_dist(0.0, 1.0);
        bool take_action = uniform_dist(rng_) < frequency;
        
        if (take_action) {
            std::string main_action = scenario.actions.empty() ? "bet" : scenario.actions.back();
            double ev = simulate_single_hand(hero_hand, board, scenario.num_opponents, main_action, 1.0);
            total_ev += ev;
        } else {
            // Simulate checking/folding
            double ev = simulate_single_hand(hero_hand, board, scenario.num_opponents, "check", 1.0);
            total_ev += ev;
        }
    }
    
    return total_ev / num_simulations;
}

double MonteCarloEngine::simulate_single_hand(const std::array<Card, 2>& hero_hand,
                                            const std::vector<Card>& board,
                                            int num_opponents,
                                            const std::string& action,
                                            double frequency) {
    // Create deck and remove known cards
    Deck deck;
    std::vector<Card> known_cards;
    known_cards.push_back(hero_hand[0]);
    known_cards.push_back(hero_hand[1]);
    for (const auto& card : board) {
        known_cards.push_back(card);
    }
    deck.remove_cards(known_cards);
    deck.shuffle(rng_);
    
    // Deal opponent hands
    auto opponent_hands = deal_opponent_hands(deck, num_opponents);
    
    // Calculate current equity
    double equity = calculate_equity(hero_hand, opponent_hands, board);
    
    // Simulate action outcome
    return simulate_action_outcome(action, equity, 2.0);
}

std::vector<std::array<Card, 2>> MonteCarloEngine::deal_opponent_hands(Deck& deck, int num_opponents) {
    std::vector<std::array<Card, 2>> opponent_hands;
    
    for (int i = 0; i < num_opponents; ++i) {
        if (deck.remaining_cards() >= 2) {
            std::array<Card, 2> hand = {deck.deal(), deck.deal()};
            opponent_hands.push_back(hand);
        }
    }
    
    return opponent_hands;
}

double MonteCarloEngine::calculate_equity(const std::array<Card, 2>& hero_hand,
                                        const std::vector<std::array<Card, 2>>& opponent_hands,
                                        const std::vector<Card>& board) {
    if (opponent_hands.empty()) {
        return 0.5; // Default equity
    }
    
    // For preflop, estimate based on hand strength
    if (board.empty()) {
        // Simplified preflop equity calculation
        int hero_rank_sum = static_cast<int>(hero_hand[0].rank) + static_cast<int>(hero_hand[1].rank);
        double base_equity = std::min(0.9, hero_rank_sum / 30.0);
        
        // Adjust for number of opponents
        double adjusted_equity = base_equity * (1.0 / (1.0 + opponent_hands.size() * 0.15));
        
        return std::max(0.1, std::min(0.9, adjusted_equity));
    }
    
    // For postflop, use hand evaluation
    if (board.size() >= 5) {
        HandValue hero_value = HandEvaluator::evaluate_best_hand(hero_hand, board);
        
        int wins = 0;
        int total = 0;
        
        for (const auto& opponent_hand : opponent_hands) {
            HandValue opponent_value = HandEvaluator::evaluate_best_hand(opponent_hand, board);
            int comparison = HandEvaluator::compare_hands(hero_value, opponent_value);
            
            if (comparison > 0) wins++;
            else if (comparison == 0) wins += 0.5; // Split pot
            total++;
        }
        
        return total > 0 ? static_cast<double>(wins) / total : 0.5;
    }
    
    // Default equity for incomplete boards
    return 0.4;
}

double MonteCarloEngine::simulate_action_outcome(const std::string& action, double equity, double pot_odds) {
    if (action == "fold") {
        return 0.0;
    } else if (action == "check" || action == "call") {
        return equity - 0.5; // Win/lose 1 unit
    } else if (action == "bet" || action == "raise" || action == "open" || action == "3bet" || action == "4bet" || action == "cbet") {
        // Betting actions - win more when we win, lose more when we lose
        double win_amount = pot_odds;
        double lose_amount = -1.0;
        
        return equity * win_amount + (1.0 - equity) * lose_amount;
    }
    
    return 0.0; // Default
}

std::array<Card, 2> MonteCarloEngine::parse_starting_hand(const std::string& hand_str) {
    if (hand_str.empty()) {
        return {Card(Rank::ACE, Suit::HEARTS), Card(Rank::KING, Suit::SPADES)};
    }
    
    // Helper function to convert character to rank
    auto char_to_rank = [](char c) -> Rank {
        switch (c) {
            case '2': return Rank::TWO;
            case '3': return Rank::THREE;
            case '4': return Rank::FOUR;
            case '5': return Rank::FIVE;
            case '6': return Rank::SIX;
            case '7': return Rank::SEVEN;
            case '8': return Rank::EIGHT;
            case '9': return Rank::NINE;
            case 'T': return Rank::TEN;
            case 'J': return Rank::JACK;
            case 'Q': return Rank::QUEEN;
            case 'K': return Rank::KING;
            case 'A': return Rank::ACE;
            default: return Rank::ACE;
        }
    };
    
    if (hand_str.length() == 2) {
        // Pocket pair like "AA", "KK", "22"
        Rank rank = char_to_rank(hand_str[0]);
        return {Card(rank, Suit::HEARTS), Card(rank, Suit::SPADES)};
    } else if (hand_str.length() == 3) {
        // Two card hand like "AKs", "72o", "AKo"
        Rank rank1 = char_to_rank(hand_str[0]);
        Rank rank2 = char_to_rank(hand_str[1]);
        char suited_char = hand_str[2];
        
        if (suited_char == 's') {
            // Suited
            return {Card(rank1, Suit::HEARTS), Card(rank2, Suit::HEARTS)};
        } else {
            // Offsuit
            return {Card(rank1, Suit::HEARTS), Card(rank2, Suit::SPADES)};
        }
    }
    
    // Default fallback
    return {Card(Rank::ACE, Suit::HEARTS), Card(Rank::KING, Suit::SPADES)};
}

} // namespace poker
