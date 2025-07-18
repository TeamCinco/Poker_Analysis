#pragma once

#include "types.hpp"
#include "hand_evaluator.hpp"
#include "deck.hpp"
#include <random>
#include <memory>
#include <vector>
#include <thread>
#include <atomic>
#include <future>

namespace poker {

class MonteCarloEngine {
public:
    MonteCarloEngine(const Config& config = Config{});
    ~MonteCarloEngine();
    
    // Main simulation functions
    SimulationResult simulate_hand(const std::string& hand_str, 
                                  int num_opponents, 
                                  int num_simulations = 1000000);
    
    SimulationResult simulate_hand_vs_range(const std::string& hand_str,
                                           const std::vector<std::string>& opponent_range,
                                           int num_simulations = 1000000);
    
    // Batch simulation for multiple hands
    std::vector<SimulationResult> simulate_multiple_hands(
        const std::vector<std::string>& hands,
        int num_opponents,
        int num_simulations_per_hand = 1000000);
    
    // Real-time simulation with early termination
    SimulationResult simulate_with_confidence(const std::string& hand_str,
                                            int num_opponents,
                                            double target_confidence = 0.95,
                                            int max_simulations = 10000000);
    
    // Performance monitoring
    PerformanceStats get_performance_stats() const;
    void reset_performance_stats();
    
    // Configuration
    void set_config(const Config& config);
    const Config& get_config() const;

private:
    Config config_;
    mutable std::atomic<uint64_t> total_simulations_;
    mutable std::atomic<uint64_t> total_time_ns_;
    mutable std::atomic<uint64_t> cache_hits_;
    mutable std::atomic<uint64_t> cache_misses_;
    
    // Thread pool for parallel execution
    std::vector<std::thread> thread_pool_;
    std::atomic<bool> shutdown_requested_;
    
    // Thread-local storage for random number generators
    thread_local static std::mt19937_64 rng_;
    thread_local static ThreadLocalData thread_data_;
    
    // Core simulation functions
    SimulationResult run_single_threaded_simulation(const std::array<Card, 2>& hero_hand,
                                                   int num_opponents,
                                                   int num_simulations);
    
    SimulationResult run_parallel_simulation(const std::array<Card, 2>& hero_hand,
                                            int num_opponents,
                                            int num_simulations);
    
    // Worker thread function
    void worker_thread_function(const std::array<Card, 2>& hero_hand,
                               int num_opponents,
                               int simulations_per_thread,
                               std::promise<SimulationResult> result_promise);
    
    // Single simulation iteration
    int simulate_single_hand(const std::array<Card, 2>& hero_hand,
                            int num_opponents,
                            Deck& deck,
                            std::mt19937_64& rng);
    
    // SIMD-optimized batch simulation
    void simulate_batch_simd(const std::array<Card, 2>& hero_hand,
                           int num_opponents,
                           int batch_size,
                           std::array<int, 8>& results);
    
    // Calculate confidence intervals
    void calculate_confidence_intervals(SimulationResult& result) const;
    
    // Adaptive sample size determination
    int determine_optimal_sample_size(const std::array<Card, 2>& hero_hand,
                                    int num_opponents,
                                    double target_confidence) const;
    
    // Initialize thread pool
    void initialize_thread_pool();
    void shutdown_thread_pool();
    
    // Variance reduction techniques
    double apply_variance_reduction(const SimulationResult& raw_result) const;
    
    // Cache for frequently simulated scenarios
    struct CacheKey {
        std::array<Card, 2> hero_hand;
        int num_opponents;
        int num_simulations;
        
        bool operator==(const CacheKey& other) const {
            return hero_hand == other.hero_hand && 
                   num_opponents == other.num_opponents &&
                   num_simulations == other.num_simulations;
        }
    };
    
    struct CacheKeyHash {
        std::size_t operator()(const CacheKey& key) const {
            std::size_t h1 = std::hash<uint64_t>{}(
                (static_cast<uint64_t>(key.hero_hand[0]) << 8) | key.hero_hand[1]);
            std::size_t h2 = std::hash<int>{}(key.num_opponents);
            std::size_t h3 = std::hash<int>{}(key.num_simulations);
            return h1 ^ (h2 << 1) ^ (h3 << 2);
        }
    };
    
    std::unordered_map<CacheKey, SimulationResult, CacheKeyHash> simulation_cache_;
    mutable std::mutex cache_mutex_;
    
    // Validate simulation results
    bool validate_simulation_result(const SimulationResult& result) const;
    
    // Statistical utilities
    double calculate_standard_error(double win_rate, uint64_t sample_size) const;
    double calculate_confidence_interval(double mean, double std_error, double confidence_level) const;
    
    // Performance timing
    class Timer {
    public:
        Timer();
        ~Timer();
        uint64_t elapsed_ns() const;
        
    private:
        std::chrono::high_resolution_clock::time_point start_time_;
    };
};

} // namespace poker
