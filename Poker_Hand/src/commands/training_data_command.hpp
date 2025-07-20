#pragma once

#include "command_base.hpp"
#include "../ui/progress_display.hpp"
#include <filesystem>
#include <chrono>

namespace poker {

/**
 * @brief Command for generating comprehensive poker training data
 * 
 * This command generates extensive training data for poker analysis,
 * including hand equity calculations, position ranges, and scenario analysis.
 * The generated data can be used for machine learning models and analysis tools.
 */
class TrainingDataCommand : public CommandBase {
public:
    void execute() override;
    
    std::string get_name() const override {
        return "data";
    }
    
    std::string get_description() const override {
        return "Generate comprehensive poker training data";
    }

private:
    /**
     * @brief Create the default configuration for training data generation
     */
    Config create_training_config() const;
    
    /**
     * @brief Display initial information about the training data generation
     */
    void display_training_info() const;
    
    /**
     * @brief Display final statistics after completion
     */
    void display_completion_stats(const std::chrono::minutes& duration, const PerformanceStats& stats) const;
};

} // namespace poker
