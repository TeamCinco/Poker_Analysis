"""
Modular Poker Hand Trainer - Main Application Entry Point

A streamlit-based poker hand trainer that helps users learn optimal 
preflop opening ranges based on position.
"""

from config.settings import configure_page
from training.training_engine import TrainingEngine

def main():
    """Main application entry point."""
    # Configure Streamlit page settings
    configure_page()
    
    # Initialize and start the training engine
    trainer = TrainingEngine()
    trainer.start_training()

if __name__ == "__main__":
    main()
