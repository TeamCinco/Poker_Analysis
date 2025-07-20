# Modular Poker Hand Trainer

A highly modular, streamlit-based poker hand trainer that helps users learn optimal preflop opening ranges based on position. Built with clean architecture principles and extreme modularity (all files under 150 lines).

## 🏗️ Architecture Overview

The application is organized into focused, single-responsibility modules:

```
Trainer/
├── app.py                          # Main entry point (20 lines)
├── config/                         # Configuration & settings
│   ├── settings.py                 # App configuration (58 lines)
│   └── position_options.py         # Position mappings (35 lines)
├── data/                          # Hand ranges & training data
│   ├── hand_ranges.py             # Chart 1 hand data (57 lines)
│   ├── chart_data.py              # Chart 2 hand data (48 lines)
│   └── position_filters.py        # Position filtering logic (47 lines)
├── core/                          # Core business logic
│   └── session_manager.py         # Session state management (98 lines)
├── ui/                           # User interface components
│   ├── card_display.py           # Visual card rendering (101 lines)
│   └── components.py             # Reusable UI elements (78 lines)
├── training/                     # Training workflow
│   ├── answer_processor.py       # Answer handling (58 lines)
│   └── training_engine.py        # Main training loop (65 lines)
└── utils/                        # Utility functions
```

## 🚀 Quick Start

### Setup
```bash
# Install dependencies
pip install -r requirements.txt

# Run the application
streamlit run app.py
```

### Mobile Access
```bash
# For mobile access on same WiFi network
streamlit run app.py --server.address 0.0.0.0
```
Then access via the Network URL shown in terminal.

## ✨ Features

- **Clean Modular Architecture**: Each file has single responsibility, under 150 lines
- **Visual Card Display**: Beautiful card rendering with proper suits and colors
- **Comprehensive Position Training**: All specific positions for 6-max and 9-max tables
- **Multiple Action Types**: Opening Raise, 3-Bet, and 4-Bet scenarios
- **Excel Data Integration**: Uses comprehensive poker range data from generated Excel files
- **Real-time Scoring**: Track accuracy and progress
- **Responsive Design**: Works great on desktop and mobile
- **Easy to Extend**: Modular design makes adding new features simple

## 🎯 Training Modes

### **Table Sizes**
- **6-Max Tables**: UTG, HJ, CO, BTN, SB, BB positions
- **9-Max Tables**: UTG, UTG1, MP1, MP2, HJ, CO, BTN, SB, BB positions

### **Action Types**
- **Opening Raise**: Learn optimal opening ranges for each position
- **3-Bet**: Master 3-betting decisions across positions  
- **4-Bet**: Practice advanced 4-betting scenarios

### **Training Examples**
- **6-max UTG Opening Raise**: Tight opening ranges from early position
- **9-max Button 3-Bet**: Aggressive 3-betting from the button
- **6-max Small Blind 4-Bet**: Advanced 4-betting from the small blind
- **Any Position/Action Combination**: Over 45 different training scenarios available

## 🏛️ Architecture Benefits

### **Maintainability**
- Single responsibility principle
- Clear separation of concerns
- Easy to locate and modify specific functionality

### **Scalability**
- Easy to add new training modes
- Simple to extend with new position types
- Modular components can be reused

### **Testing**
- Each module can be tested independently
- Clear interfaces between components
- Isolated business logic

### **Future Extensions**
Ready for enhancements like:
- 3-Bet and 4-Bet training scenarios
- Multi-position statistics tracking
- Advanced hand range scenarios
- Tournament vs Cash game modes

## 📁 Module Details

### **Configuration Layer** (`config/`)
- `settings.py`: Application constants, UI settings, color schemes
- `position_options.py`: Position mappings and display names

### **Data Layer** (`data/`)
- `hand_ranges.py`: Chart 1 position-based hand ranges
- `chart_data.py`: Chart 2 alternative training data
- `position_filters.py`: Logic for filtering hands by position

### **Core Layer** (`core/`)
- `session_manager.py`: Streamlit session state management

### **UI Layer** (`ui/`)
- `card_display.py`: Visual card rendering with CSS styling
- `components.py`: Reusable UI components (buttons, selectors, feedback)

### **Training Layer** (`training/`)
- `answer_processor.py`: Handle user answers and scoring
- `training_engine.py`: Main training workflow orchestration

## 🎮 How to Use

1. **Select Position**: Choose which position range to train
2. **View Hand**: See the displayed poker hand
3. **Make Decision**: Click FOLD or PLAY based on optimal strategy  
4. **Get Feedback**: See if you were correct and learn the reasoning
5. **Continue**: Progress through randomized hands
6. **Track Progress**: Monitor your accuracy percentage

## 🔧 Technical Notes

- **Framework**: Streamlit for rapid UI development
- **State Management**: Centralized session state handling
- **Styling**: Custom CSS for card visualization
- **Data Structure**: Tuple-based hand representation (hand, position, action)
- **Randomization**: Shuffled hand presentation for varied training

Built with clean code principles and extreme modularity for long-term maintainability and extensibility.
