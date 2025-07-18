"""
Configuration settings for the Poker Range Trainer
"""

from dataclasses import dataclass
from typing import Dict, List
import os

@dataclass
class TrainingConfig:
    """Configuration for training modes"""
    TRAINING_MODES = [
        "Opening Ranges",
        "3-Bet Ranges", 
        "4-Bet Ranges",
        "Calling Ranges",
        "Mixed Training"
    ]
    
    POSITIONS = [
        "UTG", "MP", "CO", "BTN", "SB", "BB", "Random"
    ]
    
    STACK_DEPTHS = {
        "min": 20,
        "max": 200,
        "default": 100,
        "step": 10
    }
    
    SCORING = {
        "correct_answer": 10,
        "streak_bonus": 5,
        "max_streak_bonus": 50
    }

@dataclass
class DatabaseConfig:
    """Database configuration"""
    DB_PATH = "data/poker_trainer.db"
    BACKUP_PATH = "data/backups/"
    
    # Ensure directories exist
    def __post_init__(self):
        os.makedirs(os.path.dirname(self.DB_PATH), exist_ok=True)
        os.makedirs(self.BACKUP_PATH, exist_ok=True)

@dataclass
class AppConfig:
    """Main application configuration"""
    APP_NAME = "Poker Range Trainer"
    VERSION = "2.0.0"
    PAGE_ICON = "🃏"
    
    # Server settings for multi-user support
    SERVER_HOST = "localhost"
    SERVER_PORT = 8501
    
    # UI Settings
    THEME = {
        "primary_color": "#0F4C3A",
        "secondary_color": "#2E8B57", 
        "accent_color": "#FFD700",
        "background_color": "#1a1a1a",
        "text_color": "#ffffff"
    }

# Global configuration instances
training_config = TrainingConfig()
database_config = DatabaseConfig()
app_config = AppConfig()
