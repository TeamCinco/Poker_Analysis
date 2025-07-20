#include "training_data_command.hpp"
#include <iostream>

namespace poker {

void TrainingDataCommand::execute() {
    std::cout << "=== Generating Training Data ===" << std::endl;
    
    Config config = create_training_config();
    PokerAnalyzer analyzer = create_analyzer(config);
    analyzer.set_progress_callback(ui::ProgressDisplay::progress_callback);
    
    // Create output directory
    std::filesystem::create_directories("output");
    
    display_training_info();
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    try {
        analyzer.generate_training_data("output");
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::minutes>(end_time - start_time);
        
        PerformanceStats stats = analyzer.get_performance_stats();
        display_completion_stats(duration, stats);
        
    } catch (const std::exception& e) {
        std::cerr << "Error generating training data: " << e.what() << std::endl;
        throw;
    }
    
    std::cout << std::endl;
}

Config TrainingDataCommand::create_training_config() const {
    Config config = create_default_config();
    config.min_simulations_per_hand = 1000000;
    return config;
}

void TrainingDataCommand::display_training_info() const {
    std::cout << "Generating comprehensive poker training data..." << std::endl;
    std::cout << "This may take 30-60 minutes depending on your hardware." << std::endl;
    std::cout << std::endl;
}

void TrainingDataCommand::display_completion_stats(const std::chrono::minutes& duration, const PerformanceStats& stats) const {
    std::cout << std::endl;
    std::cout << "Training data generation completed in " << duration.count() << " minutes!" << std::endl;
    
    std::cout << "Final Performance Stats:" << std::endl;
    std::cout << "  Total simulations: " << stats.total_simulations << std::endl;
    std::cout << "  Average simulations per second: " << std::fixed << std::setprecision(0) << stats.simulations_per_second << std::endl;
}

} // namespace poker
