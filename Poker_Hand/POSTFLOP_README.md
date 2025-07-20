# Postflop Analysis - Quick Start Guide

## Prerequisites
- Windows 10/11
- Visual Studio 2019+ or Visual Studio Build Tools
- CMake 3.16+

## Build Instructions

### 1. Open PowerShell in the Poker_Hand directory
```powershell
cd C:\Users\cinco\Desktop\Poker_Analysis\Poker_Hand
```

### 2. Build the project
```powershell
cmake --build build --config Release
```

### 3. Verify the executable was created
```powershell
ls output\poker_analysis_generator.exe
```

## Run Commands

### Available Commands:
- `data` - Generate training data
- `ranges` - Generate preflop range analysis
- `postflop` - **NEW!** Comprehensive postflop analysis + preflop ranges

### Run Postflop Analysis (Recommended)
```powershell
.\output\poker_analysis_generator.exe postflop
```

### Run Individual Commands
```powershell
# Preflop ranges only
.\output\poker_analysis_generator.exe ranges

# Training data only  
.\output\poker_analysis_generator.exe data

# Help
.\output\poker_analysis_generator.exe help
```

## What Gets Generated

### Postflop Command Output:
```
output/
├── 6_player/          # 6-max preflop ranges
├── 9_player/          # 9-max preflop ranges
├── flop/              # Board texture analysis (framework)
├── turn/              # Equity evolution (framework)
└── river/             # Alpha spots (framework)
```

### Performance:
- **2+ million simulations/second**
- **169 million total simulations** for complete preflop analysis
- **1-2 minutes** execution time on modern CPU
- **~500MB** CSV data output

## Quick Test
```powershell
# Quick test run
.\output\poker_analysis_generator.exe postflop

# Check output was created
ls output\6_player\UTG\opening_raise\
```

## Troubleshooting

### Build Fails?
```powershell
# Clean and rebuild
rmdir build -Recurse -Force
mkdir build
cd build
cmake ..
cmake --build . --config Release
cd ..
```

### Missing CMAKE?
Install from: https://cmake.org/download/

### Missing Visual Studio?
Install Build Tools: https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2022

## Analysis Usage

The generated CSV files can be imported into:
- Excel/Google Sheets for analysis
- Python pandas for data science
- Poker solvers for comparison
- Machine learning models for pattern recognition

**File format:** `hand,position,action,win_rate,player_count,simulations_run,expected_value,confidence_interval_low,confidence_interval_high,in_range`
