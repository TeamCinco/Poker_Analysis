# 🎯 Poker Analysis Suite

> **Advanced poker analysis and training ecosystem combining high-performance C++ engines with interactive Python applications**

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Build Status](https://img.shields.io/badge/Build-Passing-brightgreen.svg)](https://github.com/TeamCinco/Poker_Analysis)
[![Language](https://img.shields.io/badge/Language-C%2B%2B17%20%7C%20Python%203.8%2B-blue.svg)](https://github.com/TeamCinco/Poker_Analysis)

## 📋 Table of Contents

- [🎯 Overview](#-overview)
- [🏗️ Architecture](#️-architecture)
- [🚀 Quick Start](#-quick-start)
- [📦 Modules](#-modules)
  - [🔥 High-Performance Poker Engine (C++)](#-high-performance-poker-engine-c)
  - [🎓 Interactive Poker Trainer (Python)](#-interactive-poker-trainer-python)
  - [📊 Poker Session Tracker (Python)](#-poker-session-tracker-python)
- [🔧 Installation](#-installation)
- [💻 Usage Examples](#-usage-examples)
- [🤝 Contributing](#-contributing)
- [📄 License](#-license)

## 🎯 Overview

This comprehensive poker analysis suite applies quantitative analysis principles to poker - treating it like financial markets with extractable rules from game theory, probability, and strategic patterns. The system combines:

- **High-performance C++ Monte Carlo engine** for ultra-fast hand analysis
- **Interactive Python training applications** for skill development
- **Session tracking and analytics** for performance monitoring
- **Cross-platform compatibility** with optimized performance

## 🏗️ Architecture

```
Poker_Analysis/
├── 🔥 Poker_Hand/          # High-Performance C++ Engine
│   ├── src/                # Source code (C++17)
│   ├── build/              # Build artifacts
│   └── output/             # Generated analysis data
├── 🎓 Poker_Trainer/       # Interactive Python Trainer
│   ├── components/         # UI components
│   ├── data/              # Training data & databases
│   └── streaming/         # Real-time features
└── 📊 Python-Poker-Tracker/ # Session Analytics
    ├── analytics.py       # Statistical analysis
    ├── display.py         # Visualization
    └── poker_tracker.py   # Main application
```

## 🚀 Quick Start

### Prerequisites
- **Windows**: Visual Studio 2019+ or MinGW
- **Linux/macOS**: GCC 7+ or Clang 10+
- **Python**: 3.8 or higher
- **CMake**: 3.16 or higher

### 5-Minute Setup
```bash
# 1. Clone the repository
git clone https://github.com/TeamCinco/Poker_Analysis.git
cd Poker_Analysis

# 2. Build the C++ engine
cd Poker_Hand
./build.bat  # Windows
# or
make         # Linux/macOS

# 3. Install Python dependencies
cd ../Poker_Trainer
pip install -r requirements.txt

# 4. Run your first analysis
cd ../Poker_Hand/build/Release
./poker_analysis_generator demo
```

## 📦 Modules

### 🔥 High-Performance Poker Engine (C++)

**Location**: `Poker_Hand/`

Ultra-fast Monte Carlo poker analysis engine with SIMD optimization and multi-threading support.

#### ✨ Features
- **10M+ simulations/second** on modern hardware
- **Cross-platform compatibility** (Windows, Linux, macOS)
- **SIMD optimization** (AVX2, AVX-512 support)
- **Multi-threaded analysis** (scales with CPU cores)
- **Comprehensive hand analysis** (equity, ranges, scenarios)

#### 🚀 Quick Start
```bash
cd Poker_Hand

# Build the engine
./build.bat          # Windows
make                 # Linux/macOS

# Run demo analysis
cd build/Release
./poker_analysis_generator demo

# Generate full training data
./poker_analysis_generator data

# Performance benchmark
./poker_analysis_generator perf
```

#### 📊 Output Data
Generated files in `output/` directory:
- `hand_equity_data.json` - Complete hand equity analysis
- `position_ranges.json` - GTO ranges by position
- `scenario_analysis.json` - 3-bet/4-bet scenarios
- `stack_depth_adjustments.json` - Stack depth analysis

#### 🔧 Configuration
The engine automatically detects and utilizes:
- CPU core count for parallel processing
- SIMD capabilities (AVX2, AVX-512)
- Available system memory
- Compiler optimizations

---

### 🎓 Interactive Poker Trainer (Python)

**Location**: `Poker_Trainer/`

Comprehensive poker training application with real-time analysis, range training, and performance tracking.

#### ✨ Features
- **Interactive range training** with GTO solutions
- **Real-time hand analysis** powered by C++ engine
- **Progress tracking** with detailed analytics
- **Streaming capabilities** for live training
- **SQLite database** for persistent storage

#### 🚀 Quick Start
```bash
cd Poker_Trainer

# Install dependencies
pip install -r requirements.txt

# Initialize database
python -c "from utils.database import init_database; init_database()"

# Launch the trainer
python run_app.py

# Alternative: Direct launch
python app.py
```

#### 🔐 Login System
The trainer includes user management:
- **Registration**: Create new user accounts
- **Login**: Secure session management
- **Progress tracking**: User-specific analytics
- **Data backup**: Automatic session backups

#### 📊 Training Modules
- **Range Trainer**: Practice preflop ranges
- **Equity Calculator**: Real-time equity analysis
- **Session Review**: Analyze past performance
- **Feedback System**: Immediate performance feedback

#### 🌐 Streaming Features
```bash
# Start streaming server
python streaming/stream_server.py

# Connect with WebSocket client
python streaming/websocket_client.py
```

---

### 📊 Poker Session Tracker (Python)

**Location**: `Python-Poker-Tracker/`

Comprehensive session tracking and analytics system for poker performance monitoring.

#### ✨ Features
- **Session tracking** with detailed statistics
- **Performance analytics** with trend analysis
- **Bankroll management** with risk assessment
- **Visual displays** with charts and graphs
- **Quantitative analysis** applying financial metrics

#### 🚀 Quick Start
```bash
cd Python-Poker-Tracker

# Launch the main tracker
python main.py

# View analytics dashboard
python analytics.py

# Display performance charts
python display.py

# Specific modules
python poker_tracker.py    # Main tracking interface
python alpha_display.py    # Alpha metrics display
python quant_display.py    # Quantitative analysis
```

#### 📈 Analytics Features
- **Alpha Decay Analysis**: Performance degradation over time
- **Quantitative Metrics**: Sharpe ratio, volatility, drawdown
- **Session Analytics**: Win rate, profit/loss tracking
- **Deposit Management**: Bankroll tracking and analysis

#### 🔐 User Management
- **Menu System**: Interactive user interface
- **Session Management**: Start/stop tracking
- **Data Persistence**: Automatic saving
- **Export Capabilities**: CSV/JSON export

## 🔧 Installation

### System Requirements
- **CPU**: Multi-core processor (Intel/AMD)
- **RAM**: 8GB+ recommended
- **Storage**: 2GB free space
- **OS**: Windows 10+, Ubuntu 18.04+, macOS 10.14+

### Dependencies

#### C++ Engine
```bash
# Windows
# Visual Studio 2019+ with C++ tools
# CMake 3.16+

# Linux
sudo apt-get install build-essential cmake g++ libstdc++6

# macOS
brew install cmake gcc
```

#### Python Applications
```bash
# Install Python dependencies
pip install -r requirements.txt

# Common dependencies
pip install streamlit pandas numpy matplotlib sqlite3 websockets
```

## 💻 Usage Examples

### Basic Hand Analysis
```bash
# Analyze pocket aces against 3 opponents
cd Poker_Hand/build/Release
./poker_analysis_generator analyze AA 3 1000000

# Batch analyze multiple hands
./poker_analysis_generator batch "AA,KK,AKs,AKo" 2 500000
```

### Training Session
```python
# Start a training session
cd Poker_Trainer
python run_app.py

# The web interface will open at http://localhost:8501
```

### Session Tracking
```python
# Log a new session
cd Python-Poker-Tracker
python poker_tracker.py

# View analytics
python analytics.py
```

## 🤝 Contributing

We welcome contributions! Please see our [Contributing Guidelines](CONTRIBUTING.md) for details.

### Development Setup
```bash
# Fork and clone the repository
git clone https://github.com/YOUR_USERNAME/Poker_Analysis.git
cd Poker_Analysis

# Create a feature branch
git checkout -b feature/amazing-feature

# Make your changes and commit
git commit -m "Add amazing feature"

# Push to your fork and create a pull request
git push origin feature/amazing-feature
```

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

## 📞 Support

- **Issues**: [GitHub Issues](https://github.com/TeamCinco/Poker_Analysis/issues)
- **Documentation**: [Wiki](https://github.com/TeamCinco/Poker_Analysis/wiki)
- **Discussions**: [GitHub Discussions](https://github.com/TeamCinco/Poker_Analysis/discussions)

---

**Built with ❤️ by Team Cinco**

*"Applying quantitative analysis to poker - extracting rules from game theory, probability, and strategic patterns."*
