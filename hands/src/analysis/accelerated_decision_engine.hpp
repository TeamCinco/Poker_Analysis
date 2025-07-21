#pragma once

#include "../core/types.hpp"
#include "../engine/monte_carlo_engine.hpp"
#include "../engine/thread_pool.hpp"
#include "../engine/simd_optimizer.hpp"
#include "decision_engine.hpp"
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <atomic>
#include <mutex>
#include <chrono>
#include <queue>

namespace poker {

// Accelerated version of the decision engine using parallel processing and SIMD
class AcceleratedDecisionEngine {
public:
    AcceleratedDecisionEngine(int num_threads = 0); // 0 = auto-detect
    ~AcceleratedDecisionEngine() = default;
    
    // Main discovery methods - same interface as DynamicStrategyEngine
    void discover_all_strategies();
    void discover_strategy_for_table_size(int table_size);
    
    // Export discovered strategies to CSV
    void export_strategies_to_csv(int table_size);
    
    // Get progress information
    int get_total_decisions() const { return total_decisions_; }
    int get_completed_decisions() const { return completed_decisions_.load(); }
    
    // Performance metrics
    double get_throughput_per_second() const;
    void print_performance_stats() const;
    
private:
    // Core components
    std::unique_ptr<ThreadPool> thread_pool_;
    SIMDOptimizer simd_optimizer_;
    DecisionPointGenerator decision_generator_;
    MonteCarloEngine monte_carlo_;
    
    // Results storage
    std::unordered_map<int, std::vector<DecisionResult>> table_results_;
    
    // Progress tracking (thread-safe)
    std::atomic<int> total_decisions_{0};
    std::atomic<int> completed_decisions_{0};
    mutable std::mutex progress_mutex_;
    
    // Performance tracking
    std::chrono::steady_clock::time_point start_time_;
    std::chrono::steady_clock::time_point end_time_;
    
    // Configuration
    static const std::vector<double> FINE_GRAINED_FREQUENCIES;
    static const int SIMULATIONS_PER_FREQUENCY;
    static const int BATCH_SIZE; // Process decisions in batches for better cache locality
    
    // Parallel processing methods
    DecisionResult test_decision_point_accelerated(const DecisionPoint& decision);
    std::vector<DecisionResult> process_decision_batch(
        const std::vector<DecisionPoint>& batch);
    
    // Batch optimization
    void optimize_decision_batches(std::vector<DecisionPoint>& decisions);
    
    // Memory management
    void pre_allocate_memory_pools(int table_size);
    void clear_memory_pools();
    
    // Helper functions
    void clear_results_for_table(int table_size);
    void print_progress(const std::string& message);
    std::string get_output_directory(int table_size);
    void create_output_directories();
    
    // Thread-safe progress updates
    void increment_completed_decisions(int count = 1);
    
    // SIMD-optimized frequency testing
    DecisionResult find_optimal_frequency_simd(const DecisionPoint& decision);
    
    // Validate decision points in parallel
    std::vector<DecisionPoint> filter_valid_decisions_parallel(
        const std::vector<DecisionPoint>& decisions);
};

// Memory pool for efficient allocation during parallel processing
class DecisionMemoryPool {
public:
    DecisionMemoryPool(size_t initial_capacity = 10000);
    ~DecisionMemoryPool();
    
    // Get pre-allocated memory for results
    double* get_frequency_results_buffer(size_t size);
    double* get_equity_buffer(size_t size);
    
    // Return buffers (for reuse)
    void return_buffer(double* buffer);
    
private:
    std::vector<std::vector<double>> frequency_buffers_;
    std::vector<std::vector<double>> equity_buffers_;
    std::queue<double*> available_freq_buffers_;
    std::queue<double*> available_equity_buffers_;
    std::mutex buffer_mutex_;
    
    void expand_buffer_pool();
};

// Batch processor for optimal work distribution
class BatchProcessor {
public:
    BatchProcessor(int num_threads);
    
    // Process decision batches across multiple threads
    std::vector<DecisionResult> process_batches_parallel(
        const std::vector<DecisionPoint>& decisions,
        AcceleratedDecisionEngine* engine);
    
private:
    int num_threads_;
    
    // Optimize batch sizes based on decision complexity
    std::vector<std::vector<DecisionPoint>> create_optimal_batches(
        const std::vector<DecisionPoint>& decisions);
    
    // Estimate processing time for a decision
    double estimate_processing_time(const DecisionPoint& decision);
};

} // namespace poker
