#pragma once

#include <string>
#include <chrono>
#include <iomanip>

namespace poker {
namespace ui {

/**
 * @brief Progress display utilities for long-running operations
 * 
 * This class provides functionality for displaying progress bars,
 * timing information, and performance statistics during poker analysis.
 */
class ProgressDisplay {
public:
    /**
     * @brief Default progress callback function for poker analysis
     * 
     * @param completed Number of completed operations
     * @param total Total number of operations
     * @param current_hand Current hand being processed
     */
    static void progress_callback(int completed, int total, const std::string& current_hand);
    
    /**
     * @brief Print performance results in a formatted manner
     * 
     * @param total_simulations Total number of simulations performed
     * @param duration_ms Duration in milliseconds
     * @param simulations_per_second Performance metric
     * @param cache_hit_rate Cache efficiency percentage
     */
    static void print_performance_results(
        long long total_simulations,
        long long duration_ms,
        double simulations_per_second,
        double cache_hit_rate
    );
    
    /**
     * @brief Print hand analysis results in a formatted manner
     * 
     * @param hand_name Name of the hand being analyzed
     * @param combinations Number of combinations
     * @param equity_vs_random Equity against random opponents
     * @param equity_vs_tight Equity against tight opponents
     * @param equity_vs_loose Equity against loose opponents
     * @param analysis_time_ms Analysis time in milliseconds
     */
    static void print_hand_analysis(
        const std::string& hand_name,
        int combinations,
        double equity_vs_random,
        double equity_vs_tight,
        double equity_vs_loose,
        long long analysis_time_ms
    );
};

} // namespace ui
} // namespace poker
