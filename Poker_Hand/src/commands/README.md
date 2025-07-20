# Adding New Commands - Developer Guide

This guide shows how to add new poker simulation commands to the modular architecture.

## Quick Steps to Add a New Command

### 1. Create Command Files

Create two files in the `src/commands/` directory:
- `your_command.hpp` (header file)  
- `your_command.cpp` (implementation file)

### 2. Implement Command Class

```cpp
// your_command.hpp
#pragma once
#include "command_base.hpp"

namespace poker {

class YourCommand : public CommandBase {
public:
    void execute() override;
    
    std::string get_name() const override {
        return "yourcommand";  // CLI command name
    }
    
    std::string get_description() const override {
        return "Description of what your command does";
    }

private:
    // Add any private helper methods here
};

} // namespace poker
```

```cpp
// your_command.cpp
#include "your_command.hpp"
#include <iostream>

namespace poker {

void YourCommand::execute() {
    std::cout << "=== Your Command Execution ===" << std::endl;
    
    // Create configuration
    Config config = create_default_config();
    // Customize config as needed
    
    // Create analyzer
    PokerAnalyzer analyzer = create_analyzer(config);
    
    // Your simulation logic here
    // ...
    
    std::cout << "Command completed!" << std::endl;
}

} // namespace poker
```

### 3. Register Command

Add your command to `command_registry.cpp`:

```cpp
#include "your_command.hpp"  // Add this include

void CommandRegistry::register_all_commands() {
    register_command(std::make_unique<TrainingDataCommand>());
    register_command(std::make_unique<YourCommand>());  // Add this line
}
```

### 4. Update Build System

Add your source file to `CMakeLists.txt`:

```cmake
set(SOURCES
    # ... existing files ...
    src/commands/your_command.cpp  # Add this line
)
```

### 5. Build and Test

```bash
cd Poker_Hand
cmake --build build --config Release
.\build\Release\poker_analysis_generator.exe help
```

Your new command will now appear in the usage and be available via:
```bash
.\build\Release\poker_analysis_generator.exe yourcommand
```

## Available Utilities

Your command can use these built-in utilities:

### Configuration
```cpp
Config config = create_default_config();
config.min_simulations_per_hand = 500000;  // Customize as needed
```

### Analyzer Creation
```cpp
PokerAnalyzer analyzer = create_analyzer(config);
analyzer.set_progress_callback(ui::ProgressDisplay::progress_callback);
```

### UI Output
```cpp
ui::ConsoleOutput::print_banner();
ui::ProgressDisplay::print_performance_results(total, duration_ms, sps, hit_rate);
```

## Example: Range Analysis Command

Here's a complete example of a new range analysis command:

```cpp
// range_analysis_command.hpp
#pragma once
#include "command_base.hpp"

namespace poker {

class RangeAnalysisCommand : public CommandBase {
public:
    void execute() override;
    
    std::string get_name() const override {
        return "ranges";
    }
    
    std::string get_description() const override {
        return "Analyze poker hand ranges for different positions";
    }

private:
    void analyze_position_ranges();
};

} // namespace poker
```

```cpp
// range_analysis_command.cpp
#include "range_analysis_command.hpp"
#include <iostream>

namespace poker {

void RangeAnalysisCommand::execute() {
    std::cout << "=== Range Analysis ===" << std::endl;
    
    Config config = create_default_config();
    config.min_simulations_per_hand = 100000;
    
    PokerAnalyzer analyzer = create_analyzer(config);
    
    analyze_position_ranges();
}

void RangeAnalysisCommand::analyze_position_ranges() {
    std::vector<std::string> positions = {"UTG", "MP", "CO", "BTN", "SB", "BB"};
    
    for (const auto& position : positions) {
        std::cout << "Analyzing " << position << " range..." << std::endl;
        // Your range analysis logic here
    }
}

} // namespace poker
```

That's it! The modular architecture makes adding new commands simple and clean.
