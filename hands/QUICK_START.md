# 🚀 QUICK START GUIDE

**Get running in 2 minutes!**

## Step 1: Build Both Systems
```bash
cd hands
./build.bat
```
✅ This creates TWO executables in `hands/build/Release/`

## Step 2: Navigate to Executables
```bash
cd build/Release
```

## Step 3: Choose Your Analysis

### 🏃‍♂️ Quick Test (Fast Mode)
```bash
# Test the new dynamic system (fast mode)
./dynamic_poker_analyzer.exe 6 --fast
```
⏰ **Runtime:** ~2-5 minutes  
📁 **Output:** `output/dynamic_6max/`

### 📊 Traditional Analysis  
```bash
# Run original system (all table sizes)
./monte_carlo_poker.exe
```
⏰ **Runtime:** ~5-15 minutes  
📁 **Output:** `output/6max/`, `output/7max/`, etc.

### 🔬 Full Mathematical Discovery
```bash
# Run new boundary-driven system (comprehensive)
./dynamic_poker_analyzer.exe
```
⏰ **Runtime:** ~20-45 minutes  
📁 **Output:** `output/dynamic_6max/`, `output/dynamic_7max/`, etc.

## 📋 Key Commands Summary

| Command | Purpose | Time | Output |
|---------|---------|------|--------|
| `./dynamic_poker_analyzer.exe 6 --fast` | Quick test | ~2-5 min | 6-max strategies only |
| `./monte_carlo_poker.exe 6` | Traditional 6-max | ~3-5 min | Standard analysis |
| `./dynamic_poker_analyzer.exe 6` | Full 6-max discovery | ~8-15 min | All profitable plays |
| `./monte_carlo_poker.exe` | All tables traditional | ~5-15 min | Complete standard analysis |
| `./dynamic_poker_analyzer.exe` | All tables discovery | ~20-45 min | Complete mathematical discovery |

## 📁 Most Important Output Files

### For Quick Decisions:
- `output/dynamic_6max/profitable_strategies_only.csv` ⭐ **ONLY profitable plays**

### For Detailed Analysis:
- `output/dynamic_6max/dynamic_preflop_strategy.csv` - Opening/3betting ranges
- `output/dynamic_6max/dynamic_flop_strategy.csv` - C-betting frequencies  
- `output/dynamic_6max/dynamic_turn_strategy.csv` - Barreling decisions
- `output/dynamic_6max/dynamic_river_strategy.csv` - Final betting strategy

## 🎯 What You'll Discover

**Traditional System finds:**
```
6,AA,UTG,preflop,open,0.95,0.180,TRUE    <- Open AA 95% from UTG
6,KK,BTN,preflop,open,1.00,0.210,TRUE    <- Open KK 100% from BTN
```

**Dynamic System discovers:**
```
6,AA,UTG,preflop,open,0.90,0.185,TRUE    <- Math says 90% is better!
6,A5s,BTN,flop,As7h2c,cbet,0.80,0.065,TRUE <- Unexpected profitable cbet
6,72o,UTG,preflop,open,0.00,-0.250,FALSE <- Correctly rejects bad plays
```

## 🔥 Pro Tips

1. **Start with fast mode** to test the system: `./dynamic_poker_analyzer.exe 6 --fast`
2. **Use 6-max first** - it's the most common online format
3. **Check `profitable_strategies_only.csv`** for your core playbook
4. **Compare both systems** - they might find different optimal strategies!
5. **Run overnight** for full boundary exploration across all table sizes

## 🆘 Troubleshooting

**Can't find executable?**
```bash
# Make sure you're in the right directory
cd hands/build/Release
ls -la  # Should see both .exe files
```

**Build failed?**
```bash
# Clean and rebuild
cd hands
rm -rf build
./build.bat
```

**Want help with commands?**
```bash
./dynamic_poker_analyzer.exe --help
```

## 🎉 You're Ready!

**The math will discover your optimal poker strategy with zero assumptions!**

Start with the fast mode, then dive deeper with the full boundary exploration. The dynamic system might surprise you with strategies you never considered! 🚀
