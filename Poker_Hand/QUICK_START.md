To run the comprehensive flop analysis:

**Step 1: Build the project**
```powershell
cd Poker_Hand
build.bat
```

**Step 2: Run the analysis**
```powershell
cd build
.\Release\poker_analysis_generator flop
```

**Available Commands:**
- `boards` - Board texture classification (1-2 min)
- `flop` - Complete flop analysis with range evolution (30-60 min)
- `ranges` - Original preflop range analysis

**Output Location:**
Results saved to `Poker_Hand/output/flop_analysis/` with comprehensive CSV files for machine learning training.
