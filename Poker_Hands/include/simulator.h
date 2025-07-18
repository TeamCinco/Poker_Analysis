#pragma once
#include "hand.h"
#include "card.h"
#include <random>
#include <chrono>

namespace poker {
    class PokerSimulator {
    public:
        PokerSimulator();
        
        // Monte Carlo simulations
        std::map<HandType, int> runHandSimulation(int num_hands);
        
        // Texas Hold'em specific simulations
        double simulateHoldemWinRate(const std::vector<Card>& hole_cards, 
                                   int num_opponents = 1, 
                                   int num_simulations = 10000);
        
        // Tournament simulation
        struct TournamentResult {
            std::map<std::string, int> finishing_positions;
            std::map<std::string, double> roi;
        };
        
        TournamentResult simulateTournament(const std::vector<std::string>& players, 
                                          int num_tournaments = 1000);
        
    private:
        std::mt19937 rng_;
        std::uniform_int_distribution<> card_dist_;
        
        void initializeRNG();
        std::vector<Card> generateCommunityCards();
        Hand evaluateBestHand(const std::vector<Card>& hole_cards, 
                            const std::vector<Card>& community_cards);
    };
}