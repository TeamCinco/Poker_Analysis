#include "analyzer.h"
#include "simulator.h"
#include "platform_utils.h"
#include <iostream>
#include <chrono>
#include <stdexcept>

int main() {
    try {
        // Set console title
        poker::platform::setConsoleTitle("Poker Analytics System");
        
        // Display platform information
        std::cout << poker::platform::colorize("=== POKER ANALYTICS SYSTEM ===", poker::platform::Color::CYAN) << "\n\n";
        
        std::cout << "Platform: ";
        if (poker::platform::isWindows()) {
            std::cout << poker::platform::colorize("Windows", poker::platform::Color::BLUE);
        } else if (poker::platform::isMac()) {
            std::cout << poker::platform::colorize("macOS", poker::platform::Color::BLUE);
        } else {
            std::cout << poker::platform::colorize("Linux", poker::platform::Color::BLUE);
        }
        std::cout << "\n";
        
        std::cout << "Unicode Support: " << (poker::platform::supportsUnicode() ? 
            poker::platform::colorize("Yes", poker::platform::Color::GREEN) : 
            poker::platform::colorize("No", poker::platform::Color::YELLOW)) << "\n";
        
        std::cout << "Color Support: " << (poker::platform::supportsColors() ? 
            poker::platform::colorize("Yes", poker::platform::Color::GREEN) : 
            poker::platform::colorize("No", poker::platform::Color::YELLOW)) << "\n\n";
        
        // Get terminal size
        poker::platform::TerminalSize term_size = poker::platform::getTerminalSize();
        std::cout << "Terminal Size: " << term_size.width << "x" << term_size.height << "\n\n";
        
        poker::PokerAnalyzer analyzer;
        
        // Display hand rankings
        analyzer.displayHandRankings();
        
        // Run simulations
        std::cout << "\n" << poker::platform::colorize("Starting poker hand analysis...", poker::platform::Color::YELLOW) << "\n";
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        // Simulate hands for quick results
        auto simulated_stats = analyzer.simulateHands(500000);
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        std::cout << "\n" << poker::platform::colorize("Simulation completed in " + 
            std::to_string(duration.count()) + " ms", poker::platform::Color::GREEN) << "\n";
        
        // Display results
        analyzer.displayProbabilities(simulated_stats);
        
        // Example of individual hand evaluation
        std::cout << "\n" << poker::platform::colorize("=== EXAMPLE HAND EVALUATION ===", poker::platform::Color::CYAN) << "\n";
        
        std::vector<poker::Card> example_cards = {
            poker::Card(poker::Rank::ACE, poker::Suit::SPADES),
            poker::Card(poker::Rank::KING, poker::Suit::SPADES),
            poker::Card(poker::Rank::QUEEN, poker::Suit::SPADES),
            poker::Card(poker::Rank::JACK, poker::Suit::SPADES),
            poker::Card(poker::Rank::TEN, poker::Suit::SPADES)
        };
        
        poker::Hand example_hand(example_cards);
        poker::HandResult result = example_hand.evaluate();
        
        std::cout << poker::platform::bold("Example hand: ");
        for (const auto& card : example_cards) {
            std::cout << card.toString() << " ";
        }
        std::cout << "\n" << poker::platform::bold("Result: ") << 
            poker::platform::colorize(result.toString(), poker::platform::Color::YELLOW) << "\n";
        
        std::cout << "\n" << poker::platform::colorize("Analysis complete!", poker::platform::Color::GREEN) << "\n";
        
        return 0;
        
    } catch (const std::exception& e) {
        poker::platform::handlePlatformError("Main execution", e.what());
        return 1;
    } catch (...) {
        poker::platform::handlePlatformError("Main execution", "Unknown error occurred");
        return 1;
    }
}
