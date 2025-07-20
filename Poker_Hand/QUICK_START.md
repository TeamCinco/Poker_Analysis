# Quick Start Guide - High-Performance Poker Hand Analyzer

## Step 1: Build the System

### Option A: Easy Build (Windows)
```bash
# Navigate to the Poker_Hand directory
cd Poker_Hand

# Run the build script (PowerShell requires .\ prefix)
.\build.bat
```

### Option B: Manual Build
```bash
# Navigate to the Poker_Hand directory
cd Poker_Hand

# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
cmake --build . --config Release
```

## Step 2: Run the System

After building, you'll find the executable in the `build` directory.

### Quick Demo (2-3 minutes)
```bash
cd build
poker_analysis_generator demo
```
This will analyze 7 sample hands (AA, KK, AKs, etc.) and show you the performance.

### Performance Test (5-10 minutes)
```bash
cd build
poker_analysis_generator perf
```
This will run a performance benchmark to show you how fast your system is.

### Generate Full Training Data (30-60 minutes)
```bash
cd build
poker_analysis_generator data
```
This will generate all 169 hands with complete analysis for your Python trainer.

## Step 3: Check the Results

After running, check the `output` directory for generated files:
- `hand_equity_data.json` - Complete poker hand analysis
- `position_ranges.json` - GTO ranges by position
- `scenario_analysis.json` - 3-bet/4-bet scenarios
- `stack_depth_adjustments.json` - Stack depth adjustments

## Expected Performance on Your i9-12900K

- **Demo mode**: ~100-500ms per hand
- **Performance test**: 10-50 million simulations/second
- **Full data generation**: 30-60 minutes for all 169 hands
- **Memory usage**: <1GB peak

## Troubleshooting

### If build fails:
1. Make sure you have Visual Studio or MinGW installed
2. Install CMake from https://cmake.org/download/
3. Run `build.bat` again

### If performance is slow:
1. Make sure you built in Release mode
2. Check that all CPU cores are being used
3. Verify AVX2 support is detected

### If you get errors:
1. Check that you have enough RAM (8GB+ recommended)
2. Make sure no antivirus is blocking the executable
3. Try running as administrator

## Quick Commands Summary

```bash
# Build and run demo
cd Poker_Hand
.\build.bat
cd build
.\poker_analysis_generator demo

# Generate training data
.\poker_analysis_generator data

# ranges / pre flop

.\poker_analysis_generator ranges

.\poker_analysis_generator preflop



# Check help
.\poker_analysis_generator help
```

Your generated data files will be ready to load into your Python poker trainer!
