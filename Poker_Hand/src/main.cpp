#include "poker_analyzer.hpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <filesystem>

using namespace poker;

void print_banner() {
    std::cout << "==========================================" << std::endl;
    std::cout << "  High-Performance Poker Hand Analyzer   " << std::endl;
    std::cout << "       C++ Monte Carlo Engine            " << std::endl;
    std::cout << "==========================================" << std::endl;
    std::cout << std::endl;
}

void print_system_info() {
    std::cout << "System Information:" << std::endl;
    std::cout << "  CPU Cores: " << std::thread::hardware_concurrency() << std::endl;
    std::cout << "  Compiler: ";
#ifdef _MSC_VER
    std::cout << "MSVC " << _MSC_VER << std::endl;
#elif defined(__GNUC__)
    std::cout << "GCC " << __GNUC__ << "." << __GNUC_MINOR__ << std::endl;
#elif defined(__clang__)
    std::cout << "Clang " << __clang_major__ << "." << __clang_minor__ << std::endl;
#else
    std::cout << "Unknown" << std::endl;
#endif
    std::cout << "  SIMD Support: ";
#ifdef __AVX2__
    std::cout << "AVX2 ";
#endif
#ifdef __AVX512F__
    std::cout << "AVX-512 ";
#endif
    std::cout << std::endl;
    std::cout << "  Target Performance: 10M+ simulations/second" << std::endl;
    std::cout << "  Expected Speedup: 50-200x vs Python" << std::endl;
    std::cout << std::endl;
}

void progress_callback(int completed, int total, const std::string& current_hand) {
    double percentage = (static_cast<double>(completed) / total) * 100.0;
    std::cout << "\rProgress: " << std::fixed << std::setprecision(1) << percentage 
              << "% (" << completed << "/" << total << ") - Processing: " << current_hand << "    " << std::flush;
    
    if (completed == total) {
        std::cout << std::endl;
    }
}

void demo_single_hand_analysis() {
    std::cout << "=== Single Hand Analysis Demo ===" << std::endl;
    
    Config config;
    config.num_threads = std::thread::hardware_concurrency();
    config.min_simulations_per_hand = 100000;
    config.use_cache = true;
    config.use_simd = true;
    
    PokerAnalyzer analyzer(config);
    
    std::vector<std::string> demo_hands = {"AA", "KK", "AKs", "AKo", "22", "A7o", "72o"};
    
    for (const auto& hand : demo_hands) {
        std::cout << "\nAnalyzing " << hand << "..." << std::endl;
        
        auto start_time = std::chrono::high_resolution_clock::now();
        HandAnalysis analysis = analyzer.analyze_hand(hand, 100000);
        auto end_time = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        std::cout << "  Combinations: " << analysis.combinations << std::endl;
        std::cout << "  Equity vs Random: " << std::fixed << std::setprecision(2) << (analysis.equity_vs_random * 100) << "%" << std::endl;
        std::cout << "  Equity vs Tight: " << std::fixed << std::setprecision(2) << (analysis.equity_vs_tight * 100) << "%" << std::endl;
        std::cout << "  Equity vs Loose: " << std::fixed << std::setprecision(2) << (analysis.equity_vs_loose * 100) << "%" << std::endl;
        
        std::cout << "  VS Opponents:" << std::endl;
        for (int i = 0; i < 3; ++i) {  // Show first 3 opponents
            const auto& result = analysis.vs_opponents[i];
            std::cout << "    " << (i + 1) << " opponent(s): " 
                      << std::fixed << std::setprecision(2) << (result.win_rate * 100) << "% win rate" << std::endl;
        }
        
        std::cout << "  Analysis time: " << duration.count() << "ms" << std::endl;
    }
    
    std::cout << std::endl;
}

void demo_performance_test() {
    std::cout << "=== Performance Test ===" << std::endl;
    
    Config config;
    config.num_threads = std::thread::hardware_concurrency();
    config.min_simulations_per_hand = 1000000;
    config.use_cache = true;
    config.use_simd = true;
    
    PokerAnalyzer analyzer(config);
    
    std::vector<std::string> test_hands = {"AA", "KK", "QQ", "JJ", "AKs"};
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    for (const auto& hand : test_hands) {
        analyzer.analyze_hand(hand, 1000000);
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    PerformanceStats stats = analyzer.get_performance_stats();
    
    std::cout << "Performance Results:" << std::endl;
    std::cout << "  Total simulations: " << stats.total_simulations << std::endl;
    std::cout << "  Total time: " << duration.count() << "ms" << std::endl;
    std::cout << "  Simulations per second: " << std::fixed << std::setprecision(0) << stats.simulations_per_second << std::endl;
    std::cout << "  Cache hit rate: " << std::fixed << std::setprecision(2) << (stats.cache_hit_rate * 100) << "%" << std::endl;
    std::cout << std::endl;
}

void generate_training_data() {
    std::cout << "=== Generating Training Data ===" << std::endl;
    
    Config config;
    config.num_threads = std::thread::hardware_concurrency();
    config.min_simulations_per_hand = 1000000;
    config.use_cache = true;
    config.use_simd = true;
    
    PokerAnalyzer analyzer(config);
    analyzer.set_progress_callback(progress_callback);
    
    // Create output directory
    std::filesystem::create_directories("output");
    
    std::cout << "Generating comprehensive poker training data..." << std::endl;
    std::cout << "This may take 30-60 minutes depending on your hardware." << std::endl;
    std::cout << std::endl;
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    try {
        analyzer.generate_training_data("output");
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::minutes>(end_time - start_time);
        
        std::cout << std::endl;
        std::cout << "Training data generation completed in " << duration.count() << " minutes!" << std::endl;
        
        PerformanceStats stats = analyzer.get_performance_stats();
        std::cout << "Final Performance Stats:" << std::endl;
        std::cout << "  Total simulations: " << stats.total_simulations << std::endl;
        std::cout << "  Average simulations per second: " << std::fixed << std::setprecision(0) << stats.simulations_per_second << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error generating training data: " << e.what() << std::endl;
    }
    
    std::cout << std::endl;
}

void print_usage() {
    std::cout << "Usage: poker_analysis_generator [mode]" << std::endl;
    std::cout << "  demo    - Run single hand analysis demonstration" << std::endl;
    std::cout << "  perf    - Run performance test" << std::endl;
    std::cout << "  data    - Generate full training data (default)" << std::endl;
    std::cout << "  all     - Run all modes" << std::endl;
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    print_banner();
    print_system_info();
    
    std::string mode = "data";
    if (argc > 1) {
        mode = argv[1];
    }
    
    if (mode == "demo") {
        demo_single_hand_analysis();
    } else if (mode == "perf") {
        demo_performance_test();
    } else if (mode == "data") {
        generate_training_data();
    } else if (mode == "all") {
        demo_single_hand_analysis();
        demo_performance_test();
        generate_training_data();
    } else if (mode == "help" || mode == "--help" || mode == "-h") {
        print_usage();
    } else {
        std::cout << "Unknown mode: " << mode << std::endl;
        print_usage();
        return 1;
    }
    
    std::cout << "Program completed successfully!" << std::endl;
    return 0;
}
