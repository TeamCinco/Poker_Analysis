import os
import subprocess
import sys

def setup_directories():
    """Create necessary directories"""
    directories = [
        'data',
        'data/ranges',
        'data/backups',
        'components',
        'utils',
        'streaming',
        'config',
        'core'
    ]
    
    for directory in directories:
        os.makedirs(directory, exist_ok=True)
    
    print("✅ Directories created successfully!")

def create_sample_data():
    """Create sample range data"""
    import json
    
    # Sample preflop ranges
    preflop_ranges = {
        "UTG": {
            "raise": ["AA", "KK", "QQ", "JJ", "TT", "99", "AKs", "AKo", "AQs", "AQo", "AJs", "KQs"],
            "fold": ["88", "77", "66", "55", "44", "33", "22", "AJo", "ATs", "A9s", "KQo", "KJs"]
        },
        "BTN": {
            "raise": ["AA", "KK", "QQ", "JJ", "TT", "99", "88", "77", "66", "55", "44", "33", "22", 
                     "AKs", "AKo", "AQs", "AQo", "AJs", "AJo", "ATs", "A9s", "A8s", "A7s", "A6s", "A5s", "A4s", "A3s", "A2s"],
            "fold": ["T8o", "97o", "86o", "75o", "64o", "53o", "42o"]
        }
    }
    
    with open('data/ranges/preflop_ranges.json', 'w') as f:
        json.dump(preflop_ranges, f, indent=2)
    
    print("✅ Sample data created successfully!")

def run_streamlit():
    """Run the Streamlit app"""
    print("🚀 Starting Poker Range Trainer...")
    print("📱 App will be available at: http://localhost:8501")
    print("🔗 For mobile access, use your computer's IP address")
    
    subprocess.run([sys.executable, "-m", "streamlit", "run", "main_app.py", "--server.port=8502"])

if __name__ == "__main__":
    print("🃏 Poker Range Trainer Setup")
    print("=" * 50)
    
    setup_directories()
    create_sample_data()
    
    print("\n🎯 Setup complete! Starting the app...")
    print("=" * 50)
    
    run_streamlit()
