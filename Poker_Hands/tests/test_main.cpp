#include "card.h"
#include "hand.h"
#include "analyzer.h"
#include "simulator.h"
#include "platform_utils.h"
#include <iostream>
#include <cassert>
#include <vector>

namespace test {
    void testCardCreation() {
        std::cout << "Testing card creation..." << std::endl;
        
        poker::Card card(poker::Rank::ACE, poker::Suit::SPADES);
        assert(card.getRank() == poker::Rank::ACE);
        assert(card.getSuit() == poker::Suit::SPADES);
        
        std::string cardStr = card.toString();
        assert(!cardStr.empty());
        
        std::cout << "  Card creation test passed!" << std::endl;
    }
    
    void testHandEvaluation() {
        std::cout << "Testing hand evaluation..." << std::endl;
        
        // Test Royal Flush
        std::vector<poker::Card> royal_flush = {
            poker::Card(poker::Rank::ACE, poker::Suit::SPADES),
            poker::Card(poker::Rank::KING, poker::Suit::SPADES),
            poker::Card(poker::Rank::QUEEN, poker::Suit::SPADES),
            poker::Card(poker::Rank::JACK, poker::Suit::SPADES),
            poker::Card(poker::Rank::TEN, poker::Suit::SPADES)
        };
        
        poker::Hand hand(royal_flush);
        poker::HandResult result = hand.evaluate();
        assert(result.type == poker::HandType::ROYAL_FLUSH);
        
        std::cout << "  Hand evaluation test passed!" << std::endl;
    }
    
    void testDeckOperations() {
        std::cout << "Testing deck operations..." << std::endl;
        
        poker::Deck deck;
        assert(deck.size() == 52);
        assert(!deck.isEmpty());
        
        deck.shuffle();
        poker::Card card1 = deck.dealCard();
        poker::Card card2 = deck.dealCard();
        
        assert(deck.size() == 50);
        
        std::cout << "  Deck operations test passed!" << std::endl;
    }
    
    void testPlatformUtils() {
        std::cout << "Testing platform utilities..." << std::endl;
        
        // Test platform detection
        bool platform_detected = poker::platform::isWindows() || 
                                poker::platform::isMac() || 
                                poker::platform::isLinux();
        assert(platform_detected);
        
        // Test suit symbols
        std::string spade_symbol = poker::platform::getSuitSymbol(0);
        assert(!spade_symbol.empty());
        
        // Test terminal size
        poker::platform::TerminalSize size = poker::platform::getTerminalSize();
        assert(size.width > 0 && size.height > 0);
        
        std::cout << "  Platform utilities test passed!" << std::endl;
    }
    
    void testAnalyzer() {
        std::cout << "Testing analyzer..." << std::endl;
        
        poker::PokerAnalyzer analyzer;
        
        // Test simulation with small sample
        auto stats = analyzer.simulateHands(1000);
        assert(!stats.empty());
        
        // Check that we have all hand types
        assert(stats.find(poker::HandType::HIGH_CARD) != stats.end());
        assert(stats.find(poker::HandType::ONE_PAIR) != stats.end());
        
        std::cout << "  Analyzer test passed!" << std::endl;
    }
    
    void testSimulator() {
        std::cout << "Testing simulator..." << std::endl;
        
        poker::PokerSimulator simulator;
        
        // Test hand simulation
        auto hand_counts = simulator.runHandSimulation(1000);
        assert(!hand_counts.empty());
        
        std::cout << "  Simulator test passed!" << std::endl;
    }
}

int main() {
    std::cout << poker::platform::colorize("=== POKER ANALYTICS TESTS ===", poker::platform::Color::CYAN) << std::endl;
    std::cout << std::endl;
    
    try {
        test::testCardCreation();
        test::testHandEvaluation();
        test::testDeckOperations();
        test::testPlatformUtils();
        test::testAnalyzer();
        test::testSimulator();
        
        std::cout << std::endl;
        std::cout << poker::platform::colorize("All tests passed!", poker::platform::Color::GREEN) << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cout << std::endl;
        std::cout << poker::platform::colorize("Test failed: " + std::string(e.what()), poker::platform::Color::RED) << std::endl;
        return 1;
    } catch (...) {
        std::cout << std::endl;
        std::cout << poker::platform::colorize("Unknown test failure occurred", poker::platform::Color::RED) << std::endl;
        return 1;
    }
}
