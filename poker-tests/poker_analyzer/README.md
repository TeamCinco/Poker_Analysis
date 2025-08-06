# Poker Post-Flop Analysis Tool

*Last Updated: August 6, 2025 - 6:40 AM*

A high-performance C++ tool that calculates post-flop probabilities for every possible Texas Hold'em starting hand.

## Quick Start

### Prerequisites
- CMake 3.10 or higher
- C++17 compatible compiler (Visual Studio 2022, GCC, or Clang)

### Build & Run

1. **Navigate to the project:**
   ```bash
   cd POKER_ANALYSIS
   cd poker-tests\poker_analyzer
   ```

2. **Build the project:**
   ```bash
   mkdir build
   cd build
   cmake ..
   cmake --build . --config Release
   ```

3. **Run the analyzer:**
   ```bash
   # Windows
   .\bin\PokerFlopAnalyzer.exe
   
   # Linux/macOS  
   ./bin/PokerFlopAnalyzer
   ```

## What It Does

This is a **post-flop probability analyzer** that:
- Analyzes all **1,326 unique starting hands** in Texas Hold'em
- Simulates **10 million random flops** per hand using 24 parallel threads
- Calculates the probability of making different hand types after the flop
- Finds the most likely outcome for each starting hand combination

## Output

Creates `poker_flop_analysis.csv` in the build directory with:
- **Hand**: Starting cards (e.g., "AhKs")  
- **Best_Flop**: Most favorable flop scenario
- **Average_Strength**: Post-flop hand strength rating
- **Hand_Type**: Most likely hand type after flop
- **Statistical Data**: Win/tie probabilities

## Performance

Runtime: **~2-4 hours** on modern multi-core systems. Real-time progress displayed.

---

*Optimized post-flop probability calculation using Monte Carlo simulation and parallel processing.*