#include "analysis/accelerated_decision_engine.hpp"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    std::cout << "=== Dynamic Monte Carlo Poker Strategy Discovery System ===" << std::endl;
    std::cout << "NO HARDCODED STRATEGIES - Pure Mathematical Analysis" << std::endl;
    std::cout << "Tests EVERY decision point to discover what actually works" << std::endl;
    std::cout << std::endl;
    
    // Check command line arguments
    if (argc > 1) {
        std::string arg = argv[1];
        if (arg == "--help" || arg == "-h") {
            std::cout << "Usage: " << argv[0] << " [table_size]" << std::endl;
            std::cout << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  No arguments: Discover strategies for all table sizes (6-max through 9-max)" << std::endl;
            std::cout << "  6, 7, 8, 9:   Discover strategy for specific table size only" << std::endl;
            std::cout << "  --help, -h:   Show this help message" << std::endl;
            std::cout << std::endl;
            std::cout << "Dynamic Analysis Process:" << std::endl;
            std::cout << "1. Generate ALL possible decision points" << std::endl;
            std::cout << "2. Test 21 frequencies (0%, 5%, 10%, ..., 100%) for each decision" << std::endl;
            std::cout << "3. Find optimal frequency with highest Expected Value" << std::endl;
            std::cout << "4. Only include decisions that are mathematically profitable (EV > 0)" << std::endl;
            std::cout << std::endl;
            std::cout << "Output will be saved to:" << std::endl;
            std::cout << "  output/dynamic_6max/dynamic_preflop_strategy.csv" << std::endl;
            std::cout << "  output/dynamic_6max/dynamic_flop_strategy.csv" << std::endl;
            std::cout << "  output/dynamic_6max/dynamic_turn_strategy.csv" << std::endl;
            std::cout << "  output/dynamic_6max/dynamic_river_strategy.csv" << std::endl;
            std::cout << "  output/dynamic_6max/profitable_strategies_only.csv" << std::endl;
            std::cout << "  ... (and similar for 7max, 8max, 9max)" << std::endl;
            return 0;
        }
    }
    
    try {
        poker::AcceleratedDecisionEngine engine;
        
        if (argc > 1) {
            // Discover strategy for specific table size
            int table_size = std::stoi(argv[1]);
            
            if (table_size < 6 || table_size > 9) {
                std::cerr << "Error: Table size must be 6, 7, 8, or 9" << std::endl;
                return 1;
            }
            
            std::cout << "=== DISCOVERING OPTIMAL STRATEGY FOR " << table_size << "-MAX ===" << std::endl;
            std::cout << "Testing every decision point dynamically..." << std::endl;
            std::cout << "No assumptions - letting the math decide what works!" << std::endl;
            std::cout << std::endl;
            
            engine.discover_strategy_for_table_size(table_size);
        } else {
            // Discover strategies for all table sizes
            std::cout << "=== DISCOVERING STRATEGIES FOR ALL TABLE SIZES ===" << std::endl;
            std::cout << "This comprehensive analysis will:" << std::endl;
            std::cout << "• Test every hand (169) × every position × every decision type" << std::endl;
            std::cout << "• Analyze all board textures systematically" << std::endl;
            std::cout << "• Find optimal frequencies through pure Monte Carlo simulation" << std::endl;
            std::cout << "• Only recommend strategies that are mathematically profitable" << std::endl;
            std::cout << std::endl;
            std::cout << "⚠️  WARNING: This will take 15-30 minutes to complete ⚠️" << std::endl;
            std::cout << "But you'll get the most accurate, unbiased strategy data possible!" << std::endl;
            std::cout << std::endl;
            
            engine.discover_all_strategies();
        }
        
        std::cout << std::endl;
        std::cout << "=== STRATEGY DISCOVERY COMPLETE ===" << std::endl;
        std::cout << std::endl;
        std::cout << "🎯 WHAT YOU'VE DISCOVERED:" << std::endl;
        std::cout << "• Optimal frequencies for every profitable decision" << std::endl;
        std::cout << "• Expected values showing exactly how much each play makes/loses" << std::endl;
        std::cout << "• Complete elimination of losing strategies (EV < 0)" << std::endl;
        std::cout << "• Table size differences revealed through pure math" << std::endl;
        std::cout << std::endl;
        std::cout << "📊 RESULTS SAVED TO:" << std::endl;
        std::cout << "- output/dynamic_6max/" << std::endl;
        std::cout << "- output/dynamic_7max/" << std::endl;
        std::cout << "- output/dynamic_8max/" << std::endl;
        std::cout << "- output/dynamic_9max/" << std::endl;
        std::cout << std::endl;
        std::cout << "📋 EACH DIRECTORY CONTAINS:" << std::endl;
        std::cout << "• dynamic_preflop_strategy.csv (Opening, 3betting, calling ranges)" << std::endl;
        std::cout << "• dynamic_flop_strategy.csv    (C-betting, calling, raising frequencies)" << std::endl;
        std::cout << "• dynamic_turn_strategy.csv    (Barreling, bluffing, value betting)" << std::endl;
        std::cout << "• dynamic_river_strategy.csv   (Final betting decisions)" << std::endl;
        std::cout << "• profitable_strategies_only.csv (ONLY profitable plays - your core strategy)" << std::endl;
        std::cout << std::endl;
        std::cout << "📈 CSV COLUMNS EXPLAINED:" << std::endl;
        std::cout << "• optimal_frequency: How often to take this action (0.0 to 1.0)" << std::endl;
        std::cout << "• max_expected_value: Profit/loss per decision in big blinds" << std::endl;
        std::cout << "• is_profitable: TRUE if this decision makes money, FALSE if it loses" << std::endl;
        std::cout << "• decision_type: open, 3bet, cbet, call, raise, etc." << std::endl;
        std::cout << std::endl;
        std::cout << "🔥 EXAMPLE DISCOVERIES:" << std::endl;
        std::cout << "6,AA,UTG,preflop,,open,0.95,0.180,TRUE    <- Open AA 95% from UTG" << std::endl;
        std::cout << "9,AA,UTG,preflop,,open,0.85,0.120,TRUE    <- Open AA 85% from UTG in 9-max (tighter!)" << std::endl;
        std::cout << "6,72o,UTG,preflop,,open,0.00,-0.250,FALSE <- Never open 72o from UTG" << std::endl;
        std::cout << "6,AK,BTN,flop,As7h2c,cbet,0.75,0.085,TRUE     <- C-bet AK 75% on dry ace-high boards" << std::endl;
        std::cout << std::endl;
        std::cout << "🎉 THE MATH HAS SPOKEN!" << std::endl;
        std::cout << "No assumptions, no poker 'wisdom' - just pure mathematical optimization." << std::endl;
        std::cout << "Use the profitable_strategies_only.csv files as your core game plan!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
        return 1;
    }
    
    return 0;
}
