#include "accelerated_decision_engine.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <iomanip>
#include <chrono>
#include <algorithm>
#include <future>

namespace poker {

// Static member definitions
const std::vector<double> AcceleratedDecisionEngine::FINE_GRAINED_FREQUENCIES = {
    0.00, 0.05, 0.10, 0.15, 0.20, 0.25, 0.30, 0.35, 0.40, 0.45, 0.50,
    0.55, 0.60, 0.65, 0.70, 0.75, 0.80, 0.85, 0.90, 0.95, 1.00
};

const int AcceleratedDecisionEngine::SIMULATIONS_PER_FREQUENCY = 25000; // Reduced per freq, but SIMD compensates
const int AcceleratedDecisionEngine::BATCH_SIZE = 64; // Optimal for cache locality

AcceleratedDecisionEngine::AcceleratedDecisionEngine(int num_threads) {
    if (num_threads == 0) {
        num_threads = std::thread::hardware_concurrency();
    }
    
    thread_pool_ = std::make_unique<ThreadPool>(num_threads);
    
    std::cout << "🚀 Initialized Accelerated Decision Engine:" << std::endl;
    std::cout << "   • Threads: " << num_threads << std::endl;
    std::cout << "   • AVX2: " << (simd_optimizer_.has_avx2() ? "✓" : "✗") << std::endl;
    std::cout << "   • AVX-512: " << (simd_optimizer_.has_avx512() ? "✓" : "✗") << std::endl;
    std::cout << "   • Batch Size: " << BATCH_SIZE << std::endl;
}

void AcceleratedDecisionEngine::discover_all_strategies() {
    create_output_directories();
    
    for (int table_size = 6; table_size <= 9; ++table_size) {
        std::cout << "\n🔥 ACCELERATED ANALYSIS: " << table_size << "-max games" << std::endl;
        discover_strategy_for_table_size(table_size);
    }
    
    print_performance_stats();
    std::cout << "\n🎉 All accelerated strategy discovery complete!" << std::endl;
}

void AcceleratedDecisionEngine::discover_strategy_for_table_size(int table_size) {
    clear_results_for_table(table_size);
    
    start_time_ = std::chrono::steady_clock::now();
    
    // Generate all decision points
    auto all_decisions = decision_generator_.generate_all_decisions(table_size);
    
    // Filter valid decisions in parallel
    auto valid_decisions = filter_valid_decisions_parallel(all_decisions);
    
    total_decisions_ = static_cast<int>(valid_decisions.size());
    completed_decisions_ = 0;
    
    print_progress("🚀 Starting ACCELERATED " + std::to_string(table_size) + 
                   "-max strategy discovery...");
    print_progress("📊 Total decision points: " + std::to_string(total_decisions_));
    
    // Optimize decision ordering for better cache locality
    optimize_decision_batches(valid_decisions);
    
    // Pre-allocate memory pools
    pre_allocate_memory_pools(table_size);
    
    // Process decisions in parallel batches
    std::vector<DecisionResult> all_results;
    
    // Split decisions into batches for parallel processing
    const int batch_size = BATCH_SIZE;
    std::vector<std::future<std::vector<DecisionResult>>> futures;
    
    for (size_t start = 0; start < valid_decisions.size(); start += batch_size) {
        size_t end = std::min(start + batch_size, valid_decisions.size());
        std::vector<DecisionPoint> batch(valid_decisions.begin() + start, 
                                       valid_decisions.begin() + end);
        
        // Submit batch to thread pool
        auto future = thread_pool_->enqueue([this, batch]() {
            return process_decision_batch(batch);
        });
        futures.push_back(std::move(future));
    }
    
    // Collect results from all batches
    for (auto& future : futures) {
        auto batch_results = future.get();
        all_results.insert(all_results.end(), batch_results.begin(), batch_results.end());
    }
    
    // Wait for all threads to complete
    thread_pool_->wait_for_all();
    
    end_time_ = std::chrono::steady_clock::now();
    
    // Store results
    table_results_[table_size] = std::move(all_results);
    
    // Export to CSV
    export_strategies_to_csv(table_size);
    
    print_progress("⚡ ACCELERATED analysis complete for " + std::to_string(table_size) + "-max!");
    print_performance_stats();
    
    // Clean up memory pools
    clear_memory_pools();
}

std::vector<DecisionResult> AcceleratedDecisionEngine::process_decision_batch(
    const std::vector<DecisionPoint>& batch) {
    
    std::vector<DecisionResult> results;
    results.reserve(batch.size());
    
    for (const auto& decision : batch) {
        DecisionResult result = test_decision_point_accelerated(decision);
        results.push_back(result);
        
        increment_completed_decisions();
        
        // Progress update for large batches
        if (completed_decisions_ % 500 == 0) {
            double throughput = get_throughput_per_second();
            print_progress("⚡ Processed " + std::to_string(completed_decisions_.load()) + 
                          "/" + std::to_string(total_decisions_) + 
                          " decisions (" + std::to_string(static_cast<int>(throughput)) + " dec/sec)");
        }
    }
    
    return results;
}

DecisionResult AcceleratedDecisionEngine::test_decision_point_accelerated(const DecisionPoint& decision) {
    return find_optimal_frequency_simd(decision);
}

DecisionResult AcceleratedDecisionEngine::find_optimal_frequency_simd(const DecisionPoint& decision) {
    DecisionResult result;
    result.decision = decision;
    result.test_frequencies = FINE_GRAINED_FREQUENCIES;
    result.frequency_evs.resize(FINE_GRAINED_FREQUENCIES.size());
    
    // Create scenario for Monte Carlo testing
    Scenario scenario;
    scenario.hand = decision.hand;
    scenario.position = decision.position;
    scenario.actions = decision.action_history;
    scenario.actions.push_back(decision.decision_type);
    scenario.board = decision.board;
    scenario.street = decision.street;
    scenario.table_size = decision.table_size;
    scenario.num_opponents = decision.num_opponents;
    
    // Use SIMD-optimized batch frequency testing
    auto evs = simd_optimizer_.batch_test_frequencies(
        scenario, 
        FINE_GRAINED_FREQUENCIES, 
        SIMULATIONS_PER_FREQUENCY
    );
    
    result.frequency_evs = evs;
    
    // Find optimal frequency
    double best_ev = -999.0;
    double best_frequency = 0.0;
    
    for (size_t i = 0; i < FINE_GRAINED_FREQUENCIES.size(); ++i) {
        if (evs[i] > best_ev) {
            best_ev = evs[i];
            best_frequency = FINE_GRAINED_FREQUENCIES[i];
        }
    }
    
    result.optimal_frequency = best_frequency;
    result.max_expected_value = best_ev;
    result.total_simulations = SIMULATIONS_PER_FREQUENCY * static_cast<int>(FINE_GRAINED_FREQUENCIES.size());
    result.is_profitable = (best_ev > 0.0);
    
    return result;
}

std::vector<DecisionPoint> AcceleratedDecisionEngine::filter_valid_decisions_parallel(
    const std::vector<DecisionPoint>& decisions) {
    
    std::vector<DecisionPoint> valid_decisions;
    std::mutex valid_mutex;
    
    const int batch_size = 1000;
    std::vector<std::future<void>> futures;
    
    for (size_t start = 0; start < decisions.size(); start += batch_size) {
        size_t end = std::min(start + batch_size, decisions.size());
        
        auto future = thread_pool_->enqueue([&, start, end]() {
            std::vector<DecisionPoint> local_valid;
            
            for (size_t i = start; i < end; ++i) {
                const auto& decision = decisions[i];
                
                // Validation logic (same as original but parallelized)
                if (!decision.hand.empty() && !decision.decision_type.empty() &&
                    decision.decision_type != "fold" &&
                    !(decision.street != Street::PREFLOP && decision.hand == "72o")) {
                    local_valid.push_back(decision);
                }
            }
            
            // Merge local results thread-safely
            std::lock_guard<std::mutex> lock(valid_mutex);
            valid_decisions.insert(valid_decisions.end(), local_valid.begin(), local_valid.end());
        });
        
        futures.push_back(std::move(future));
    }
    
    // Wait for all validation to complete
    for (auto& future : futures) {
        future.get();
    }
    
    return valid_decisions;
}

void AcceleratedDecisionEngine::optimize_decision_batches(std::vector<DecisionPoint>& decisions) {
    // Sort decisions to improve cache locality and reduce memory access patterns
    std::sort(decisions.begin(), decisions.end(), [](const DecisionPoint& a, const DecisionPoint& b) {
        // Group by street first (preflop, flop, turn, river)
        if (a.street != b.street) return a.street < b.street;
        
        // Then by hand type (pairs first, then suited, then offsuit)
        if (a.hand != b.hand) return a.hand < b.hand;
        
        // Then by position
        if (a.position != b.position) return a.position < b.position;
        
        // Finally by decision type
        return a.decision_type < b.decision_type;
    });
}

void AcceleratedDecisionEngine::pre_allocate_memory_pools(int table_size) {
    // Memory pools are handled automatically by the SIMD optimizer
    // and thread pool - no additional pre-allocation needed
}

void AcceleratedDecisionEngine::clear_memory_pools() {
    // Cleanup handled automatically
}

void AcceleratedDecisionEngine::export_strategies_to_csv(int table_size) {
    std::string output_dir = get_output_directory(table_size);
    auto& results = table_results_[table_size];
    
    // Separate results by street for easier analysis
    std::vector<DecisionResult> preflop_results, flop_results, turn_results, river_results;
    
    for (const auto& result : results) {
        switch (result.decision.street) {
            case Street::PREFLOP: preflop_results.push_back(result); break;
            case Street::FLOP: flop_results.push_back(result); break;
            case Street::TURN: turn_results.push_back(result); break;
            case Street::RIVER: river_results.push_back(result); break;
        }
    }
    
    // Export function
    auto export_street_results = [&](const std::vector<DecisionResult>& street_results, 
                                     const std::string& filename) {
        std::ofstream file(output_dir + "/" + filename);
        if (!file.is_open()) {
            std::cerr << "Failed to create file: " << filename << std::endl;
            return;
        }
        
        // Write CSV header
        file << "table_size,hand,position,street,board,action_history,decision_type,"
             << "optimal_frequency,max_expected_value,is_profitable,total_simulations,"
             << "pot_size,bet_size,decision_id\n";
        
        for (const auto& result : street_results) {
            const DecisionPoint& d = result.decision;
            
            std::string history_str;
            for (size_t i = 0; i < d.action_history.size(); ++i) {
                if (i > 0) history_str += "-";
                history_str += d.action_history[i];
            }
            
            file << std::fixed << std::setprecision(6)
                 << d.table_size << ","
                 << d.hand << ","
                 << position_to_string(d.position) << ","
                 << street_to_string(d.street) << ","
                 << d.board << ","
                 << history_str << ","
                 << d.decision_type << ","
                 << result.optimal_frequency << ","
                 << result.max_expected_value << ","
                 << (result.is_profitable ? "TRUE" : "FALSE") << ","
                 << result.total_simulations << ","
                 << d.pot_size << ","
                 << d.bet_size << ","
                 << d.get_unique_id() << "\n";
        }
        
        file.close();
        std::cout << "⚡ Exported " << street_results.size() << " results to " << filename << std::endl;
    };
    
    export_street_results(preflop_results, "accelerated_preflop_strategy.csv");
    export_street_results(flop_results, "accelerated_flop_strategy.csv");
    export_street_results(turn_results, "accelerated_turn_strategy.csv");
    export_street_results(river_results, "accelerated_river_strategy.csv");
    
    // Export only profitable strategies
    std::vector<DecisionResult> profitable_results;
    std::copy_if(results.begin(), results.end(), std::back_inserter(profitable_results),
                 [](const DecisionResult& r) { return r.is_profitable; });
    
    export_street_results(profitable_results, "accelerated_profitable_strategies.csv");
}

double AcceleratedDecisionEngine::get_throughput_per_second() const {
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time_).count();
    
    if (duration == 0) return 0.0;
    
    return (completed_decisions_.load() * 1000.0) / duration;
}

void AcceleratedDecisionEngine::print_performance_stats() const {
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time_ - start_time_).count();
    double throughput = get_throughput_per_second();
    
    std::cout << "\n📈 PERFORMANCE METRICS:" << std::endl;
    std::cout << "   • Decisions Processed: " << completed_decisions_.load() << std::endl;
    std::cout << "   • Total Time: " << duration << " ms" << std::endl;
    std::cout << "   • Throughput: " << static_cast<int>(throughput) << " decisions/second" << std::endl;
    std::cout << "   • Thread Pool Size: " << thread_pool_->size() << std::endl;
    std::cout << "   • SIMD Acceleration: " << (simd_optimizer_.has_avx512() ? "AVX-512" : 
                                               simd_optimizer_.has_avx2() ? "AVX2" : "Scalar") << std::endl;
}

void AcceleratedDecisionEngine::increment_completed_decisions(int count) {
    completed_decisions_.fetch_add(count);
}

void AcceleratedDecisionEngine::clear_results_for_table(int table_size) {
    table_results_[table_size].clear();
    total_decisions_ = 0;
    completed_decisions_ = 0;
}

void AcceleratedDecisionEngine::print_progress(const std::string& message) {
    std::lock_guard<std::mutex> lock(progress_mutex_);
    auto now = std::chrono::steady_clock::now();
    auto duration = now.time_since_epoch();
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
    
    std::cout << "[" << seconds << "] " << message << std::endl;
}

std::string AcceleratedDecisionEngine::get_output_directory(int table_size) {
    return "output/accelerated_" + std::to_string(table_size) + "max";
}

void AcceleratedDecisionEngine::create_output_directories() {
    for (int table_size = 6; table_size <= 9; ++table_size) {
        std::string dir = get_output_directory(table_size);
        std::filesystem::create_directories(dir);
    }
}

} // namespace poker
