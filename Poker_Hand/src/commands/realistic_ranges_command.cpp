#include "realistic_ranges_command.hpp"
#include "../analysis/poker_analyzer.hpp"
#include "../ui/progress_display.hpp"
#include <iostream>
#include <chrono>

namespace poker {

RealisticRangesCommand::RealisticRangesCommand() = default;

void RealisticRangesCommand::execute() {
    std::cout << "=== Realistic Poker Range Analysis ===" << std::endl;
    std::cout << "This will generate comprehensive CSV data for 6-max and 9-max games" << std::endl;
    std::cout << "using realistic opening ranges for each position." << std::endl << std::endl;
    
    print_help();
    
    // Create analyzer with high-performance config
    Config config;
    config.num_threads = 0;  // Use all available cores
    config.use_simd = true;
    config.use_cache = true;
    config.min_simulations_per_hand = 1000000;  // 1M simulations per hand for accuracy
    
    PokerAnalyzer analyzer(config);
    
    // Set up progress callback
    analyzer.set_progress_callback([this](int completed, int total, const std::string& current_hand) {
        show_progress(completed, total, current_hand);
    });
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    try {
        std::cout << "Starting realistic range analysis..." << std::endl;
        std::cout << "Configuration: " << config.min_simulations_per_hand << " simulations per hand" << std::endl;
        std::cout << "Output directory: ./output/" << std::endl << std::endl;
        
        // Generate the comprehensive realistic range analysis
        analyzer.generate_realistic_analysis_data("output", config.min_simulations_per_hand);
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);
        
        std::cout << std::endl << "=== Analysis Complete ===" << std::endl;
        std::cout << "Total execution time: " << duration.count() << " seconds" << std::endl;
        std::cout << "Output structure created in ./output/" << std::endl;
        std::cout << "  - 6_player/[position]/[action]/{high,low}_winrate_hands.csv" << std::endl;
        std::cout << "  - 9_player/[position]/[action]/{high,low}_winrate_hands.csv" << std::endl;
        
        // Display performance stats
        auto stats = analyzer.get_performance_stats();
        std::cout << std::endl << "Performance Statistics:" << std::endl;
        std::cout << "  Total simulations: " << stats.total_simulations << std::endl;
        std::cout << "  Simulations/second: " << static_cast<int>(stats.simulations_per_second) << std::endl;
        std::cout << "  Cache hit rate: " << (stats.cache_hit_rate * 100.0) << "%" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error during analysis: " << e.what() << std::endl;
        throw;
    }
}

std::string RealisticRangesCommand::get_name() const {
    return "ranges";
}

std::string RealisticRangesCommand::get_description() const {
    return "Generate realistic poker range analysis with CSV export for 6-max and 9-max games";
}

void RealisticRangesCommand::print_help() const {
    std::cout << "This analysis will generate:" << std::endl;
    std::cout << "• Opening ranges for each position (UTG, HJ, CO, BTN, SB, BB)" << std::endl;
    std::cout << "• 3-bet and 4-bet ranges for advanced scenarios" << std::endl;
    std::cout << "• Separate analysis for 6-max and 9-max table sizes" << std::endl;
    std::cout << "• High win rate (>60%) and low win rate (≤60%) hand categorization" << std::endl;
    std::cout << "• All 169 starting hands with realistic range marking" << std::endl << std::endl;
    
    std::cout << "Expected ranges:" << std::endl;
    std::cout << "• UTG 6-max: ~20 hands (tight, premium only)" << std::endl;
    std::cout << "• BTN 6-max: ~75 hands (widest, includes bluffs)" << std::endl;
    std::cout << "• UTG 9-max: ~12 hands (very tight)" << std::endl;
    std::cout << "• BTN 9-max: ~85 hands (extremely wide for stealing)" << std::endl << std::endl;
    
    std::cout << "Estimated time: 15-30 minutes depending on CPU performance" << std::endl;
    std::cout << "Press Ctrl+C to cancel at any time..." << std::endl << std::endl;
}

void RealisticRangesCommand::show_progress(int completed, int total, const std::string& current_item) {
    if (total == 0) return;
    
    static int last_percent = -1;
    int percent = (completed * 100) / total;
    
    if (percent != last_percent) {
        std::cout << "\rProgress: [";
        int bar_length = 40;
        int filled = (completed * bar_length) / total;
        
        for (int i = 0; i < bar_length; ++i) {
            if (i < filled) {
                std::cout << "=";
            } else if (i == filled && completed < total) {
                std::cout << ">";
            } else {
                std::cout << " ";
            }
        }
        
        std::cout << "] " << percent << "% (" << completed << "/" << total << ") " << current_item;
        std::cout.flush();
        
        last_percent = percent;
        
        if (completed == total) {
            std::cout << std::endl;
        }
    }
}

} // namespace poker
