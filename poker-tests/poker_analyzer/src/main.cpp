#include "poker_analyzer.hpp"
#include <iostream>

int main() {
    std::cout << "Texas Hold'em Flop Analysis Tool\n";
    std::cout << "=================================\n\n";
    
    PokerAnalyzer analyzer;
    analyzer.analyzeAllHands();
    
    std::cout << "\nPress Enter to exit...";
    std::cin.get();
    
    return 0;
}