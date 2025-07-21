# Monte Carlo Poker Analysis System

A comprehensive Monte Carlo simulation system with TWO powerful approaches to discover optimal poker strategies through pure mathematical analysis.

## 🎯 TWO ANALYSIS SYSTEMS

### 1. **Original System** - Traditional Scenario-Based Analysis
- Predefined scenarios for common poker situations
- Tests 169 starting hands × 6 positions × betting sequences × table sizes
- Frequency testing: 0%, 25%, 50%, 75%, 100%
- Fast and focused analysis

### 2. **Dynamic Boundary-Driven System** - Pure Mathematical Discovery
- **NO hardcoded strategies** - only boundary parameters
- Generates ALL possible combinations within defined limits
- Tests 11 frequencies: 0%, 10%, 20%, ..., 100%
- Recursive action tree exploration
- **ONLY saves profitable strategies** (EV > 0.01)

## 🚀 Quick Start

### Build Both Systems
```bash
# Navigate to the hands directory
cd hands

# Build everything (creates both executables)
./build.bat
```

After building, you'll have TWO executables in `hands/build/Release/`:
- `monte_carlo_poker.exe` - Original system
- `dynamic_poker_analyzer.exe` - New boundary-driven system

### Navigate to Built Executables
```bash
cd build/Release
```

## 📊 Running Analysis

### Original System (Traditional)
```bash
# Analyze all table sizes (6-max through 9-max)
./monte_carlo_poker.exe

# Analyze specific table size only
./monte_carlo_poker.exe 6        # 6-max only
./monte_carlo_poker.exe 9        # 9-max only
```

**Output Location:** `output/6max/`, `output/7max/`, etc.

### Dynamic Boundary-Driven System (NEW!)
```bash
# Full boundary exploration (all table sizes)
./dynamic_poker_analyzer.exe

# Specific table size boundary exploration
./dynamic_poker_analyzer.exe 6              # 6-max only
./dynamic_poker_analyzer.exe 9              # 9-max only

# Fast mode (reduced simulations for testing)
./dynamic_poker_analyzer.exe --fast         # All tables, fast
./dynamic_poker_analyzer.exe 6 --fast       # 6-max, fast
./dynamic_poker_analyzer.exe --help         # Show detailed help
```

**Output Location:** `output/dynamic_6max/`, `output/dynamic_7max/`, etc.

## 🔍 Key Differences

| Feature | Original System | Dynamic System |
|---------|----------------|----------------|
| **Approach** | Predefined scenarios | Boundary parameters + exploration |
| **Strategies** | Tests known situations | Discovers ALL profitable combinations |
| **Hardcoding** | Some scenario lists | Zero - pure mathematical discovery |
| **Frequencies** | 5 test points (0%, 25%, 50%, 75%, 100%) | 11 test points (0% to 100% by 10%) |
| **Output** | All tested scenarios | **ONLY profitable strategies** |
| **Speed** | Faster (focused scenarios) | Slower (comprehensive exploration) |
| **Discovery** | Expected strategies | **Unexpected profitable plays** |

## 📈 What Each System Discovers

### Original System Answers:
- Should AA open 95% or 85% from UTG in 6-max vs 9-max?
- What's the optimal c-bet frequency with AK on dry boards?
- How often should you 3-bet KQs from the button?

### Dynamic System Discovers:
- **Every profitable decision** within boundary limits
- Optimal frequencies for **unexpected hand/position combos**
- **Action sequences** that traditional analysis might miss
- **Board texture** specific strategies
- **Table size differences** revealed through pure math

## 📁 Output Files

### Original System
```
output/6max/preflop_analysis.csv
output/6max/flop_analysis.csv  
output/6max/turn_analysis.csv
output/6max/river_analysis.csv
```

### Dynamic System  
```
output/dynamic_6max/dynamic_preflop_strategy.csv
output/dynamic_6max/dynamic_flop_strategy.csv
output/dynamic_6max/dynamic_turn_strategy.csv  
output/dynamic_6max/dynamic_river_strategy.csv
output/dynamic_6max/profitable_strategies_only.csv  ⭐ **KEY FILE**
```

## 🎯 Recommended Usage

### For Traditional Poker Analysis:
Use the **Original System** - faster and covers standard situations

### For Mathematical Discovery:
Use the **Dynamic System** - finds every profitable play within your boundaries

### For Best Results:
**Run both systems** and compare findings!

## 🔬 Dynamic System Example

The dynamic system sets boundaries like:
- **Ranks:** A, K, Q, J, T, 9, 8, 7, 6, 5, 4, 3, 2
- **Positions:** UTG, HJ, CO, BTN, SB, BB  
- **Actions:** open, 3bet, 4bet, call, fold, cbet, check, raise, bet

Then it:
1. Generates **ALL** possible combinations (AA from UTG, 72o from BTN, etc.)
2. Tests **each combination** at multiple frequencies
3. **Only saves** combinations with positive expected value
4. **Recursively explores** profitable action sequences

**Result:** Pure mathematical discovery of what actually works!

## ⚡ Performance

- **Original System:** ~5-15 minutes for complete analysis
- **Dynamic System (Full):** ~20-45 minutes for comprehensive exploration  
- **Dynamic System (Fast):** ~5-10 minutes with `--fast` flag

## 💡 Key Insight

**The math tells us what works rather than assuming traditional poker wisdom.**

Both systems use Monte Carlo simulation to find optimal strategies, but the dynamic system goes further by exploring the **entire mathematical space** within your defined boundaries.

## 🎉 Example Discoveries

**Original System:**
```
6,AA,UTG,preflop,open,0.95,0.180,TRUE    <- Open AA 95% from UTG
```

**Dynamic System:**  
```
6,AA,UTG,preflop,open,0.90,0.185,TRUE    <- Math found 90% is better!
6,72o,UTG,preflop,open,0.00,-0.250,FALSE <- Never open 72o (rejected)
6,A5s,BTN,flop,As7h2c,cbet,0.80,0.065,TRUE <- Unexpected profitable cbet
```

**The dynamic system might discover strategies the original system never tested!**
