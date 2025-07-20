#pragma once

#include "../analysis/poker_analyzer.hpp"
#include <string>
#include <thread>

namespace poker {

/**
 * @brief Base class for all simulation commands
 * 
 * This abstract class provides the interface that all command implementations
 * must follow, enabling easy extensibility for new simulations.
 */
class CommandBase {
public:
    virtual ~CommandBase() = default;
    
    /**
     * @brief Execute the command's main functionality
     */
    virtual void execute() = 0;
    
    /**
     * @brief Get the command name used for CLI invocation
     */
    virtual std::string get_name() const = 0;
    
    /**
     * @brief Get a description of what this command does
     */
    virtual std::string get_description() const = 0;

protected:
    /**
     * @brief Create a default configuration for poker analysis
     * 
     * This provides a common configuration baseline that commands can use
     * and customize as needed for their specific requirements.
     */
    Config create_default_config() const {
        Config config;
        config.num_threads = std::thread::hardware_concurrency();
        config.use_cache = true;
        config.use_simd = true;
        return config;
    }
    
    /**
     * @brief Create an analyzer instance with the given configuration
     */
    PokerAnalyzer create_analyzer(const Config& config) const {
        return PokerAnalyzer(config);
    }
};

} // namespace poker
