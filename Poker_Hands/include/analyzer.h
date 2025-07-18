#pragma once
#include "hand.h"
#include <map>
#include <vector>

namespace poker {
    struct HandStatistics {
        int count;
        double probability;
        double percentage;
        std::string odds;
    };

    class PokerAnalyzer {
    public:
        PokerAnalyzer();
        
        // Exact probability calculations
        std::map<HandType, HandStatistics> calculateExactProbabilities();
        
        // Simulation-based analysis
        std::map<HandType, HandStatistics> simulateHands(int num_simulations = 100000);
        
        // Pre-flop analysis for Texas Hold'em
        std::map<std::string, double> analyzePreFlopStrength(int num_simulations = 50000);
        
        // Hand vs Hand analysis
        double calculateWinProbability(const Hand& hand1, const Hand& hand2, int simulations = 10000);
        
        // Display results
        void displayHandRankings();
        void displayProbabilities(const std::map<HandType, HandStatistics>& stats);
        void displayPreFlopAnalysis(const std::map<std::string, double>& results);
        
    private:
        std::vector<std::pair<Rank, Rank>> getPopularStartingHands();
        std::string formatStartingHand(Rank rank1, Rank rank2, bool suited = false);
        Hand getBestFiveCardHand(const std::vector<Card>& cards);
    };
}