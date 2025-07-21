# Monte Carlo Poker Analysis System

A clean, focused Monte Carlo simulation system that discovers optimal poker strategies through pure mathematical analysis. Tests every decision point from preflop through river for 6-max through 9-max games.

## Key Features

- **Pure Monte Carlo**: No hardcoded strategies - discovers optimal play through simulation
- **Complete Coverage**: All 169 starting hands × 6 positions × betting sequences × table sizes
- **Multi-Table Analysis**: Separate analysis for 6-max, 7-max, 8-max, and 9-max games
- **Frequency Testing**: Tests 0%, 25%, 50%, 75%, 100% frequencies and selects highest EV
- **Clean Output**: Organized CSV files ready for analysis

## What It Discovers

The system answers questions like:
- Should AA open 95% or 85% from UTG in 6-max vs 9-max?
- What's the optimal c-bet frequency with AK on dry boards?
- How often should you 3-bet KQs from the button in different game sizes?
- When should you barrel the turn after betting flop?

**The math tells us what works rather than assuming traditional poker wisdom.**

## Quick Start

### Windows
```bash
# Build the system

cd hands

./build.bat

# to run AFTER BUILDING:

PS C:\Users\cinco\Desktop\Poker_Analysis\hands\build> Release\monte_carlo_poker.exe
