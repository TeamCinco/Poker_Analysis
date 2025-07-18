#include "monte_carlo_engine.hpp"
#include <chrono>
#include <cmath>
#include <mutex>
#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <future>

namespace poker {

// Thread-local storage definitions
thread_local std::mt19937_64 MonteCarloEngine::rng_(std::random_device{}());
thread_local ThreadLocalData MonteCarloEngine::thread_data_;

MonteCarloEngine::MonteCarloEngine(const Config& config) 
    : config_(config), total_simulations_(0), total_time_ns_(0), 
      cache_hits_(0), cache_misses_(0), shutdown_requested_(false) {
    
    if (config_.num_threads == 0) {
        config_.num_threads = std::thread::hardware_concurrency();
    }
    
    initialize_thread_pool();
}

MonteCarloEngine::~MonteCarloEngine() {
    shutdown_thread_pool();
}

SimulationResult MonteCarloEngine::simulate_hand(const std::string& hand_str,
                                                int num_opponents,
                                                int num_simulations) {
    Timer timer;
    
    // Parse hand string
    auto hero_cards = parse_hand_string(hand_str);
    std::array<Card, 2> hero_hand = {hero_cards.first, hero_cards.second};
    
    // Check cache first
    if (config_.use_cache) {
        CacheKey key = {hero_hand, num_opponents, num_simulations};
        std::lock_guard<std::mutex> lock(cache_mutex_);
        
        auto it = simulation_cache_.find(key);
        if (it != simulation_cache_.end()) {
            cache_hits_++;
            return it->second;
        }
        cache_misses_++;
    }
    
    // Run simulation
    SimulationResult result;
    if (config_.num_threads == 1) {
        result = run_single_threaded_simulation(hero_hand, num_opponents, num_simulations);
    } else {
        result = run_parallel_simulation(hero_hand, num_opponents, num_simulations);
    }
    
    // Calculate confidence intervals
    calculate_confidence_intervals(result);
    
    // Cache result
    if (config_.use_cache) {
        CacheKey key = {hero_hand, num_opponents, num_simulations};
        std::lock_guard<std::mutex> lock(cache_mutex_);
        simulation_cache_[key] = result;
    }
    
    // Update performance stats
    total_simulations_ += num_simulations;
    total_time_ns_ += timer.elapsed_ns();
    
    return result;
}

SimulationResult MonteCarloEngine::simulate_hand_vs_range(const std::string& hand_str,
                                                         const std::vector<std::string>& opponent_range,
                                                         int num_simulations) {
    // For now, simulate vs random opponents
    // In production, this would specifically simulate against the given range
    return simulate_hand(hand_str, 1, num_simulations);
}

std::vector<SimulationResult> MonteCarloEngine::simulate_multiple_hands(
    const std::vector<std::string>& hands,
    int num_opponents,
    int num_simulations_per_hand) {
    
    std::vector<SimulationResult> results;
    results.reserve(hands.size());
    
    for (const auto& hand : hands) {
        results.push_back(simulate_hand(hand, num_opponents, num_simulations_per_hand));
    }
    
    return results;
}

SimulationResult MonteCarloEngine::simulate_with_confidence(const std::string& hand_str,
                                                          int num_opponents,
                                                          double target_confidence,
                                                          int max_simulations) {
    auto hero_cards = parse_hand_string(hand_str);
    std::array<Card, 2> hero_hand = {hero_cards.first, hero_cards.second};
    
    // Start with a small sample to estimate variance
    int initial_sample = std::min(10000, max_simulations);
    SimulationResult result = run_single_threaded_simulation(hero_hand, num_opponents, initial_sample);
    
    // Calculate required sample size for target confidence
    int required_samples = determine_optimal_sample_size(hero_hand, num_opponents, target_confidence);
    required_samples = std::min(required_samples, max_simulations);
    
    if (required_samples > initial_sample) {
        int additional_samples = required_samples - initial_sample;
        SimulationResult additional_result = run_single_threaded_simulation(hero_hand, num_opponents, additional_samples);
        
        // Combine results
        uint64_t total_sims = result.total_simulations + additional_result.total_simulations;
        result.win_rate = (result.win_rate * result.total_simulations + 
                          additional_result.win_rate * additional_result.total_simulations) / total_sims;
        result.tie_rate = (result.tie_rate * result.total_simulations + 
                          additional_result.tie_rate * additional_result.total_simulations) / total_sims;
        result.loss_rate = 1.0 - result.win_rate - result.tie_rate;
        result.total_simulations = total_sims;
    }
    
    calculate_confidence_intervals(result);
    return result;
}

PerformanceStats MonteCarloEngine::get_performance_stats() const {
    PerformanceStats stats;
    stats.total_simulations = total_simulations_.load();
    stats.total_time_seconds = total_time_ns_.load() / 1e9;
    stats.simulations_per_second = (stats.total_time_seconds > 0) ? 
        stats.total_simulations / stats.total_time_seconds : 0.0;
    stats.cache_hits = cache_hits_.load();
    stats.cache_misses = cache_misses_.load();
    stats.cache_hit_rate = (stats.cache_hits + stats.cache_misses > 0) ?
        static_cast<double>(stats.cache_hits) / (stats.cache_hits + stats.cache_misses) : 0.0;
    
    return stats;
}

void MonteCarloEngine::reset_performance_stats() {
    total_simulations_ = 0;
    total_time_ns_ = 0;
    cache_hits_ = 0;
    cache_misses_ = 0;
}

void MonteCarloEngine::set_config(const Config& config) {
    config_ = config;
    if (config_.num_threads == 0) {
        config_.num_threads = std::thread::hardware_concurrency();
    }
}

const Config& MonteCarloEngine::get_config() const {
    return config_;
}

SimulationResult MonteCarloEngine::run_single_threaded_simulation(const std::array<Card, 2>& hero_hand,
                                                                 int num_opponents,
                                                                 int num_simulations) {
    SimulationResult result;
    result.total_simulations = num_simulations;
    
    uint64_t wins = 0;
    uint64_t ties = 0;
    
    Deck deck;
    
    for (int i = 0; i < num_simulations; ++i) {
        int outcome = simulate_single_hand(hero_hand, num_opponents, deck, rng_);
        if (outcome > 0) wins++;
        else if (outcome == 0) ties++;
    }
    
    result.win_rate = static_cast<double>(wins) / num_simulations;
    result.tie_rate = static_cast<double>(ties) / num_simulations;
    result.loss_rate = 1.0 - result.win_rate - result.tie_rate;
    
    return result;
}

SimulationResult MonteCarloEngine::run_parallel_simulation(const std::array<Card, 2>& hero_hand,
                                                          int num_opponents,
                                                          int num_simulations) {
    int num_threads = config_.num_threads;
    int simulations_per_thread = num_simulations / num_threads;
    int remaining_simulations = num_simulations % num_threads;
    
    std::vector<std::future<SimulationResult>> futures;
    std::vector<std::thread> threads;
    futures.reserve(num_threads);
    threads.reserve(num_threads);
    
    // Launch worker threads
    for (int i = 0; i < num_threads; ++i) {
        int thread_simulations = simulations_per_thread + (i < remaining_simulations ? 1 : 0);
        
        std::promise<SimulationResult> promise;
        futures.push_back(promise.get_future());
        
        threads.emplace_back(&MonteCarloEngine::worker_thread_function, this,
                           hero_hand, num_opponents, thread_simulations, 
                           std::move(promise));
    }
    
    // Collect results
    SimulationResult combined_result;
    combined_result.total_simulations = num_simulations;
    
    uint64_t total_wins = 0;
    uint64_t total_ties = 0;
    
    try {
        for (auto& future : futures) {
            SimulationResult thread_result = future.get();
            total_wins += static_cast<uint64_t>(thread_result.win_rate * thread_result.total_simulations);
            total_ties += static_cast<uint64_t>(thread_result.tie_rate * thread_result.total_simulations);
        }
    } catch (const std::exception& e) {
        // If any thread fails, fall back to single-threaded simulation
        for (auto& thread : threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
        return run_single_threaded_simulation(hero_hand, num_opponents, num_simulations);
    }
    
    // Wait for all threads to complete
    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    
    combined_result.win_rate = static_cast<double>(total_wins) / num_simulations;
    combined_result.tie_rate = static_cast<double>(total_ties) / num_simulations;
    combined_result.loss_rate = 1.0 - combined_result.win_rate - combined_result.tie_rate;
    
    return combined_result;
}

void MonteCarloEngine::worker_thread_function(const std::array<Card, 2>& hero_hand,
                                             int num_opponents,
                                             int simulations_per_thread,
                                             std::promise<SimulationResult> result_promise) {
    try {
        SimulationResult result = run_single_threaded_simulation(hero_hand, num_opponents, simulations_per_thread);
        result_promise.set_value(result);
    } catch (const std::exception& e) {
        result_promise.set_exception(std::current_exception());
    }
}

int MonteCarloEngine::simulate_single_hand(const std::array<Card, 2>& hero_hand,
                                          int num_opponents,
                                          Deck& deck,
                                          std::mt19937_64& rng) {
    deck.reset_with_removed_cards(hero_hand);
    deck.shuffle(rng);
    
    // Deal opponent hands
    std::vector<std::array<Card, 2>> opponent_hands;
    for (int i = 0; i < num_opponents; ++i) {
        if (deck.remaining_cards() >= 2) {
            opponent_hands.push_back(deck.deal_hole_cards());
        }
    }
    
    // Deal board
    if (deck.remaining_cards() < 5) {
        return 0;  // Not enough cards, treat as tie
    }
    
    std::array<Card, 5> board = deck.deal_board();
    
    // Evaluate hands
    std::array<Card, 7> hero_7_cards = {hero_hand[0], hero_hand[1], 
                                       board[0], board[1], board[2], board[3], board[4]};
    uint32_t hero_rank = g_hand_evaluator.evaluate_7_cards(hero_7_cards);
    
    int better_hands = 0;
    int equal_hands = 0;
    
    for (const auto& opp_hand : opponent_hands) {
        std::array<Card, 7> opp_7_cards = {opp_hand[0], opp_hand[1],
                                          board[0], board[1], board[2], board[3], board[4]};
        uint32_t opp_rank = g_hand_evaluator.evaluate_7_cards(opp_7_cards);
        
        int comparison = g_hand_evaluator.compare_hands(hero_rank, opp_rank);
        if (comparison < 0) better_hands++;
        else if (comparison == 0) equal_hands++;
    }
    
    if (better_hands > 0) return -1;  // Loss
    if (equal_hands > 0) return 0;   // Tie
    return 1;  // Win
}

void MonteCarloEngine::calculate_confidence_intervals(SimulationResult& result) const {
    double std_error = calculate_standard_error(result.win_rate, result.total_simulations);
    double interval = calculate_confidence_interval(result.win_rate, std_error, config_.confidence_level);
    
    result.confidence_interval_low = std::max(0.0, result.win_rate - interval);
    result.confidence_interval_high = std::min(1.0, result.win_rate + interval);
}

int MonteCarloEngine::determine_optimal_sample_size(const std::array<Card, 2>& hero_hand,
                                                   int num_opponents,
                                                   double target_confidence) const {
    // Quick estimation - in practice this would use more sophisticated methods
    return std::max(100000, static_cast<int>(1000000 * target_confidence));
}

void MonteCarloEngine::initialize_thread_pool() {
    // Thread pool initialization (if needed for more advanced parallel processing)
}

void MonteCarloEngine::shutdown_thread_pool() {
    shutdown_requested_ = true;
    for (auto& thread : thread_pool_) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

double MonteCarloEngine::calculate_standard_error(double win_rate, uint64_t sample_size) const {
    if (sample_size == 0) return 0.0;
    return std::sqrt(win_rate * (1.0 - win_rate) / sample_size);
}

double MonteCarloEngine::calculate_confidence_interval(double mean, double std_error, double confidence_level) const {
    // Using normal approximation with z-score
    double z_score = (confidence_level >= 0.95) ? 1.96 : 1.645;  // 95% or 90%
    return z_score * std_error;
}

bool MonteCarloEngine::validate_simulation_result(const SimulationResult& result) const {
    return result.win_rate >= 0.0 && result.win_rate <= 1.0 &&
           result.tie_rate >= 0.0 && result.tie_rate <= 1.0 &&
           result.loss_rate >= 0.0 && result.loss_rate <= 1.0 &&
           std::abs((result.win_rate + result.tie_rate + result.loss_rate) - 1.0) < 1e-9;
}

// Timer implementation
MonteCarloEngine::Timer::Timer() : start_time_(std::chrono::high_resolution_clock::now()) {}

MonteCarloEngine::Timer::~Timer() = default;

uint64_t MonteCarloEngine::Timer::elapsed_ns() const {
    auto end_time = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time_).count();
}

} // namespace poker
