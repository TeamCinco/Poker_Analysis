#include "poker_analyzer.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <thread>
#include <future>
#include <chrono>

// Static member initialization
std::vector<int> PokerAnalyzer::straightMasks;
bool PokerAnalyzer::lookupInitialized = false;

PokerAnalyzer::PokerAnalyzer() {
    if (!lookupInitialized) {
        initializeLookupTables();
        lookupInitialized = true;
    }
}

void PokerAnalyzer::initializeLookupTables() {
    // Pre-compute straight masks for faster straight detection
    straightMasks.resize(8192, 0);  // 2^13 possible rank combinations
    
    // All possible straights
    std::vector<std::vector<int>> straights = {
        {0, 1, 2, 3, 4},    // A-2-3-4-5 (wheel)
        {1, 2, 3, 4, 5},    // 2-3-4-5-6
        {2, 3, 4, 5, 6},    // 3-4-5-6-7
        {3, 4, 5, 6, 7},    // 4-5-6-7-8
        {4, 5, 6, 7, 8},    // 5-6-7-8-9
        {5, 6, 7, 8, 9},    // 6-7-8-9-T
        {6, 7, 8, 9, 10},   // 7-8-9-T-J
        {7, 8, 9, 10, 11},  // 8-9-T-J-Q
        {8, 9, 10, 11, 12}, // 9-T-J-Q-K
        {9, 10, 11, 12, 0}  // T-J-Q-K-A (broadway)
    };
    
    for (const auto& straight : straights) {
        int mask = 0;
        for (int rank : straight) {
            mask |= (1 << rank);
        }
        straightMasks[mask] = 1;
    }
}

std::vector<Card> PokerAnalyzer::createDeck() {
    std::vector<Card> deck;
    deck.reserve(52);
    for (int rank = 0; rank < 13; rank++) {
        for (int suit = 0; suit < 4; suit++) {
            deck.emplace_back(rank, suit);
        }
    }
    return deck;
}

HandType PokerAnalyzer::evaluateHandFast(const std::array<Card, 5>& cards) {
    // Fast evaluation using bit operations
    std::array<int, 13> rankCounts{};
    std::array<int, 4> suitCounts{};
    int rankMask = 0;
    
    // Count ranks and suits, build rank mask
    for (const Card& card : cards) {
        rankCounts[card.rank]++;
        suitCounts[card.suit]++;
        rankMask |= (1 << card.rank);
    }
    
    // Check for flush
    bool isFlush = false;
    for (int count : suitCounts) {
        if (count >= 5) {
            isFlush = true;
            break;
        }
    }
    
    // Check for straight using lookup table
    bool isStraight = straightMasks[rankMask];
    
    if (isFlush && isStraight) return STRAIGHT_FLUSH;
    
    // Count pairs, trips, quads using array instead of map
    std::array<int, 5> counts{};  // counts[0] unused, counts[1-4] for pair/trip/quad counts
    for (int count : rankCounts) {
        if (count > 0 && count <= 4) {
            counts[count]++;
        }
    }
    
    if (counts[4] > 0) return FOUR_OF_KIND;
    if (counts[3] > 0 && counts[2] > 0) return FULL_HOUSE;
    if (isFlush) return FLUSH;
    if (isStraight) return STRAIGHT;
    if (counts[3] > 0) return THREE_OF_KIND;
    if (counts[2] >= 2) return TWO_PAIR;
    if (counts[2] > 0) return PAIR;
    
    return HIGH_CARD;
}

inline double PokerAnalyzer::calculateHandStrengthFast(const std::array<Card, 2>& holeCards, const std::array<Card, 3>& flop) {
    std::array<Card, 5> hand = {holeCards[0], holeCards[1], flop[0], flop[1], flop[2]};
    
    HandType handType = evaluateHandFast(hand);
    double baseScore = handType * 1000;
    
    // Simplified kicker calculation for speed
    double kickerScore = 0;
    for (const Card& card : hand) {
        kickerScore += card.rank;
    }
    
    return baseScore + kickerScore;
}

void PokerAnalyzer::workerThread(WorkerData* data) {
    FlopResult bestResult;
    bestResult.avgStrength = -1;
    bestResult.wins = 0;
    bestResult.ties = 0;
    
    std::vector<Card>& availableCards = data->availableCards;
    const int numAvailable = static_cast<int>(availableCards.size());
    
    for (int run = 0; run < data->runsPerWorker; run++) {
        // Fast random selection of 3 cards without full shuffle
        int idx1 = data->rng() % numAvailable;
        int idx2 = data->rng() % (numAvailable - 1);
        if (idx2 >= idx1) idx2++;
        int idx3 = data->rng() % (numAvailable - 2);
        if (idx3 >= std::min(idx1, idx2)) idx3++;
        if (idx3 >= std::max(idx1, idx2)) idx3++;
        
        std::array<Card, 3> currentFlop = {
            availableCards[idx1],
            availableCards[idx2],
            availableCards[idx3]
        };
        
        double strength = calculateHandStrengthFast(data->holeCards, currentFlop);
        
        if (strength > bestResult.avgStrength) {
            bestResult.flop = currentFlop;
            bestResult.avgStrength = strength;
            bestResult.wins = 1;
            bestResult.ties = 0;
        } else if (strength == bestResult.avgStrength) {
            bestResult.ties++;
        }
    }
    
    data->result = bestResult;
}

PokerAnalyzer::FlopResult PokerAnalyzer::findBestFlopParallel(const std::array<Card, 2>& holeCards) {
    std::vector<Card> deck = createDeck();
    
    // Remove hole cards from deck
    std::vector<Card> availableCards;
    availableCards.reserve(50);
    
    for (const Card& card : deck) {
        if (!(card == holeCards[0] || card == holeCards[1])) {
            availableCards.push_back(card);
        }
    }
    
    // Prepare worker data
    std::vector<WorkerData> workerData(NUM_THREADS);
    std::vector<std::thread> workers;
    workers.reserve(NUM_THREADS);
    
    const int runsPerWorker = MONTE_CARLO_RUNS / NUM_THREADS;
    
    // Start worker threads
    for (int i = 0; i < NUM_THREADS; i++) {
        workerData[i].availableCards = availableCards;
        workerData[i].holeCards = holeCards;
        workerData[i].runsPerWorker = runsPerWorker;
        workers.emplace_back(&PokerAnalyzer::workerThread, this, &workerData[i]);
    }
    
    // Wait for all workers to complete
    for (auto& worker : workers) {
        worker.join();
    }
    
    // Find best result across all workers
    FlopResult bestOverall;
    bestOverall.avgStrength = -1;
    bestOverall.wins = 0;
    bestOverall.ties = 0;
    
    for (const auto& data : workerData) {
        if (data.result.avgStrength > bestOverall.avgStrength) {
            bestOverall = data.result;
        } else if (data.result.avgStrength == bestOverall.avgStrength) {
            bestOverall.ties += data.result.ties;
        }
    }
    
    return bestOverall;
}

void PokerAnalyzer::analyzeAllHandsParallel() {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    std::ofstream csvFile("poker_flop_analysis.csv");
    csvFile << "Hand,Best_Flop,Average_Strength,Hand_Type,Wins,Ties\n";
    
    std::vector<Card> deck = createDeck();
    int handCount = 0;
    int totalHands = 52 * 51 / 2; // All unique 2-card combinations
    
    std::cout << "Analyzing " << totalHands << " possible starting hands...\n";
    std::cout << "Each hand will be tested against " << MONTE_CARLO_RUNS << " random flops using " << NUM_THREADS << " threads.\n\n";
    
    for (int i = 0; i < DECK_SIZE; i++) {
        for (int j = i + 1; j < DECK_SIZE; j++) {
            std::array<Card, 2> holeCards = {deck[i], deck[j]};
            
            handCount++;
            if (handCount % 50 == 0) {  // Update more frequently
                auto currentTime = std::chrono::high_resolution_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime);
                double handsPerSecond = handCount / (elapsed.count() + 0.001);
                
                std::cout << "Progress: " << handCount << "/" << totalHands 
                          << " (" << std::fixed << std::setprecision(1) 
                          << (100.0 * handCount / totalHands) << "%) - "
                          << std::setprecision(2) << handsPerSecond << " hands/sec\n";
            }
            
            FlopResult result = findBestFlopParallel(holeCards);
            
            // Determine hand type name
            std::string handTypeName;
            int handTypeInt = (int)(result.avgStrength / 1000);
            switch (handTypeInt) {
                case HIGH_CARD: handTypeName = "High Card"; break;
                case PAIR: handTypeName = "Pair"; break;
                case TWO_PAIR: handTypeName = "Two Pair"; break;
                case THREE_OF_KIND: handTypeName = "Three of a Kind"; break;
                case STRAIGHT: handTypeName = "Straight"; break;
                case FLUSH: handTypeName = "Flush"; break;
                case FULL_HOUSE: handTypeName = "Full House"; break;
                case FOUR_OF_KIND: handTypeName = "Four of a Kind"; break;
                case STRAIGHT_FLUSH: handTypeName = "Straight Flush"; break;
                default: handTypeName = "Unknown"; break;
            }
            
            csvFile << holeCards[0].toString() << holeCards[1].toString() << ","
                    << result.flopToString() << ","
                    << std::fixed << std::setprecision(2) << result.avgStrength << ","
                    << handTypeName << ","
                    << result.wins << ","
                    << result.ties << "\n";
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto totalTime = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
    
    csvFile.close();
    std::cout << "\nAnalysis complete! Results saved to 'poker_flop_analysis.csv'\n";
    std::cout << "Total time: " << totalTime.count() << " seconds\n";
    std::cout << "Total simulations: " << (long long)totalHands * MONTE_CARLO_RUNS << "\n";
    std::cout << "Simulations per second: " << std::fixed << std::setprecision(0) 
              << ((long long)totalHands * MONTE_CARLO_RUNS) / (double)totalTime.count() << "\n";
}

void PokerAnalyzer::analyzeAllHands() {
    analyzeAllHandsParallel();
}