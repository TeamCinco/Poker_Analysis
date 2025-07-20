#include "progress_display.hpp"
#include <iostream>
#include <iomanip>

namespace poker {
namespace ui {

void ProgressDisplay::progress_callback(int completed, int total, const std::string& current_hand) {
    double percentage = (static_cast<double>(completed) / total) * 100.0;
    std::cout << "\rProgress: " << std::fixed << std::setprecision(1) << percentage 
              << "% (" << completed << "/" << total << ") - Processing: " << current_hand << "    " << std::flush;
    
    if (completed == total) {
        std::cout << std::endl;
    }
}

void ProgressDisplay::print_performance_results(
    long long total_simulations,
    long long duration_ms,
    double simulations_per_second,
    double cache_hit_rate
) {
    std::cout << "Performance Results:" << std::endl;
    std::cout << "  Total simulations: " << total_simulations << std::endl;
    std::cout << "  Total time: " << duration_ms << "ms" << std::endl;
    std::cout << "  Simulations per second: " << std::fixed << std::setprecision(0) << simulations_per_second << std::endl;
    std::cout << "  Cache hit rate: " << std::fixed << std::setprecision(2) << (cache_hit_rate * 100) << "%" << std::endl;
    std::cout << std::endl;
}

void ProgressDisplay::print_hand_analysis(
    const std::string& hand_name,
    int combinations,
    double equity_vs_random,
    double equity_vs_tight,
    double equity_vs_loose,
    long long analysis_time_ms
) {
    std::cout << "\nAnalyzing " << hand_name << "..." << std::endl;
    std::cout << "  Combinations: " << combinations << std::endl;
    std::cout << "  Equity vs Random: " << std::fixed << std::setprecision(2) << (equity_vs_random * 100) << "%" << std::endl;
    std::cout << "  Equity vs Tight: " << std::fixed << std::setprecision(2) << (equity_vs_tight * 100) << "%" << std::endl;
    std::cout << "  Equity vs Loose: " << std::fixed << std::setprecision(2) << (equity_vs_loose * 100) << "%" << std::endl;
    std::cout << "  Analysis time: " << analysis_time_ms << "ms" << std::endl;
}

} // namespace ui
} // namespace poker
