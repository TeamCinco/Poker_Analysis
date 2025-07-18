# Poker Analytics System

A comprehensive cross-platform poker hand analysis system written in C++17 that provides statistical analysis, hand evaluation, and simulation capabilities for poker games.

## Features

### 🎯 Core Functionality
- **Hand Evaluation**: Accurately evaluates poker hands from High Card to Royal Flush
- **Statistical Analysis**: Calculates exact probabilities and runs Monte Carlo simulations
- **Texas Hold'em Support**: Pre-flop hand analysis and win rate calculations
- **Tournament Simulation**: Multi-player tournament outcome modeling

### 🖥️ Cross-Platform Support
- **Windows**: Full support with Visual Studio and MinGW
- **macOS**: Native support with Xcode and command-line tools
- **Linux**: Compatible with GCC and Clang compilers

### 🎨 Enhanced Display
- **Unicode Support**: Beautiful card symbols (♠♥♦♣) with ASCII fallback
- **Color Output**: Colored terminal output when supported
- **Platform Detection**: Automatic detection of operating system and capabilities
- **Terminal Adaptation**: Automatic adjustment to terminal size and features

## Quick Start

### Windows
```powershell
# Run the Windows build script (PowerShell)
.\build_windows.ps1

# Or run the batch file (Command Prompt)
build_windows.bat

# Or manually build:
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

### macOS/Linux
```bash
# Make the script executable (if needed)
chmod +x build_unix.sh

# Run the Unix build script
./build_unix.sh

# Or manually build:
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

## System Requirements

### Windows
- Windows 10 or later
- Visual Studio 2019/2022 or MinGW
- CMake 3.10+

### macOS
- macOS 10.14 or later
- Xcode Command Line Tools
- CMake 3.10+

### Linux
- GCC 7+ or Clang 7+
- CMake 3.10+
- Standard C++ development tools

## Building from Source

### Dependencies
- C++17 compatible compiler
- CMake 3.10 or higher
- Standard C++ library

### Build Options
```bash
# Release build (optimized)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Debug build
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Disable tests
cmake .. -DBUILD_TESTS=OFF
```

## Usage

### Running the Main Program
```bash
# Windows
build\bin\Release\poker_analytics.exe

# macOS/Linux
./build/bin/poker_analytics
```

### Running Tests
```bash
# Windows
build\bin\Release\poker_tests.exe

# macOS/Linux
./build/bin/poker_tests
```

### Sample Output
```
=== POKER ANALYTICS SYSTEM ===

Platform: Windows
Unicode Support: Yes
Color Support: Yes
Terminal Size: 120x30

=== POKER HAND RANKINGS ===
Hand Type           Description    
--------------------------------------------------
Royal Flush         A-K-Q-J-T, all same suit
Straight Flush      Five consecutive cards, same suit
Four of a Kind      Four cards of same rank
...

Running 500000 hand simulations...
Simulated 10000 hands...
...

=== POKER HAND PROBABILITIES ===
Hand Type           Count       Probability  Percentage  Odds           
---------------------------------------------------------------------------
Royal Flush         4          0.000002     0.000%      1 in 649740    
Straight Flush      36         0.000014     0.001%      1 in 72193     
Four of a Kind      624        0.000240     0.024%      1 in 4165      
...
```

## Architecture

### Core Components
- **Card & Deck**: Basic playing card representation and deck management
- **Hand**: Poker hand evaluation and comparison
- **Analyzer**: Statistical analysis and probability calculations
- **Simulator**: Monte Carlo simulations and tournament modeling
- **Platform Utils**: Cross-platform compatibility layer

### Platform Utilities
The system includes comprehensive platform utilities:
- **Display**: Unicode/ASCII card symbols with color support
- **Terminal**: Size detection and capability testing
- **File System**: Cross-platform path handling
- **Error Handling**: Platform-specific error reporting

## API Reference

### Basic Usage
```cpp
#include "analyzer.h"
#include "simulator.h"

// Create analyzer
poker::PokerAnalyzer analyzer;

// Run simulation
auto stats = analyzer.simulateHands(100000);

// Display results
analyzer.displayProbabilities(stats);
```

### Hand Evaluation
```cpp
#include "hand.h"
#include "card.h"

// Create a hand
std::vector<poker::Card> cards = {
    poker::Card(poker::Rank::ACE, poker::Suit::SPADES),
    poker::Card(poker::Rank::KING, poker::Suit::SPADES),
    // ... more cards
};

poker::Hand hand(cards);
poker::HandResult result = hand.evaluate();
```

### Platform Detection
```cpp
#include "platform_utils.h"

if (poker::platform::isWindows()) {
    // Windows-specific code
} else if (poker::platform::isMac()) {
    // macOS-specific code
} else {
    // Linux-specific code
}
```

## Testing

The system includes comprehensive tests covering:
- Card creation and operations
- Hand evaluation accuracy
- Deck shuffling and dealing
- Platform utility functions
- Statistical analysis correctness
- Cross-platform compatibility

Run tests with:
```bash
# After building
ctest

# Or run test executable directly
./build/bin/poker_tests
```

## Performance

### Benchmarks
- **Hand Evaluation**: ~1M hands/second
- **Simulation**: 500K hands in ~2-3 seconds
- **Memory Usage**: <50MB peak usage
- **Platform Overhead**: <1% performance impact

### Optimization Notes
- Release builds are ~10x faster than debug builds
- Unicode display adds minimal overhead
- Platform detection is cached for performance

## Troubleshooting

### Common Issues

#### Build Errors
- **Missing CMake**: Install CMake 3.10+
- **Compiler Not Found**: Install appropriate C++ compiler
- **Unicode Issues**: System may fall back to ASCII display

#### Runtime Issues
- **No Color Output**: Terminal may not support ANSI colors
- **Slow Performance**: Ensure using Release build
- **Memory Issues**: Reduce simulation size

### Platform-Specific Notes

#### Windows
- Use Visual Studio 2019+ for best experience
- MinGW is supported but may have limitations
- Windows Terminal recommended for Unicode support

#### macOS
- Xcode Command Line Tools required
- Homebrew recommended for CMake installation
- Terminal.app and iTerm2 both supported

#### Linux
- GCC 7+ or Clang 7+ required
- Most terminals support Unicode and colors
- Install build-essential package

## Contributing

### Code Style
- C++17 standard compliance
- 4-space indentation
- Comprehensive error handling
- Cross-platform compatibility

### Testing
- Add tests for new features
- Ensure cross-platform compatibility
- Test on multiple compilers

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments

- Built with modern C++17 features
- Cross-platform design inspired by industry best practices
- Statistical calculations based on standard poker mathematics
