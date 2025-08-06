#ifndef POKER_ANALYZER_HPP
#define POKER_ANALYZER_HPP

#include <vector>
#include <string>
#include <random>
#include <map>
#include <set>
#include <thread>
#include <atomic>
#include <mutex>
#include <chrono>
#include <array>

// Card representation: 0-12 = A,2,3,4,5,6,7,8,9,T,J,Q,K
// Suits: 0=Spades, 1=Hearts, 2=Diamonds, 3=Clubs
struct Card {
    int rank;  // 0-12
    int suit;  // 0-3
    
    // Default constructor - initializes to Ace of Spades
    Card() : rank(0), suit(0) {}
    
    Card(int r, int s) : rank(r), suit(s) {}
    
    inline std::string toString() const {
        static const char ranks[] = "A23456789TJQK";
        static const char suits[] = "SHDC";
        return std::string(1, ranks[rank]) + std::string(1, suits[suit]);
    }
    
    inline int toInt() const {
        return rank * 4 + suit;
    }
    
    inline bool operator==(const Card& other) const {
        return rank == other.rank && suit == other.suit;
    }
};

// Hand strength evaluation
enum HandType {
    HIGH_CARD = 0,
    PAIR = 1,
    TWO_PAIR = 2,
    THREE_OF_KIND = 3,
    STRAIGHT = 4,
    FLUSH = 5,
    FULL_HOUSE = 6,
    FOUR_OF_KIND = 7,
    STRAIGHT_FLUSH = 8
};

class PokerAnalyzer {
private:
    static const int MONTE_CARLO_RUNS = 10000000;  // 10M runs per hand
    static const int DECK_SIZE = 52;
    static const int NUM_THREADS = 24;  // Utilize all your cores
    
    // Fast lookup tables for evaluation
    static std::vector<int> straightMasks;
    static bool lookupInitialized;
    
    void initializeLookupTables();
    std::vector<Card> createDeck();
    HandType evaluateHandFast(const std::array<Card, 5>& cards);
    inline double calculateHandStrengthFast(const std::array<Card, 2>& holeCards, const std::array<Card, 3>& flop);
    
public:
    PokerAnalyzer();
    
    struct FlopResult {
        std::array<Card, 3> flop;
        double avgStrength;
        int wins;
        int ties;
        
        // Constructor to properly initialize
        FlopResult() : avgStrength(0.0), wins(0), ties(0) {}
        
        std::string flopToString() const {
            return flop[0].toString() + flop[1].toString() + flop[2].toString();
        }
    };
    
    struct WorkerData {
        std::vector<Card> availableCards;
        std::array<Card, 2> holeCards;
        int runsPerWorker;
        FlopResult result;
        std::mt19937 rng;
        
        WorkerData() : runsPerWorker(0), rng(std::random_device{}()) {}
    };
    
    void workerThread(WorkerData* data);
    FlopResult findBestFlopParallel(const std::array<Card, 2>& holeCards);
    void analyzeAllHands();
    void analyzeAllHandsParallel();
};

#endif // POKER_ANALYZER_HPP