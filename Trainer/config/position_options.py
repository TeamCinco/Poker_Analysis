"""Position configuration and options."""

from typing import Dict

class PositionConfig:
    """Position-based configuration for poker trainer."""
    
    POSITION_OPTIONS = {
        "all": "All Positions",
        "any_position": "Any Position (Premium Hands)",
        "mid_late_position": "Mid/Late Position",
        "late_position": "Late Position (Button/Cutoff)"
    }
    
    # Position abbreviations for display
    POSITION_DISPLAY = {
        "UTG": "Under The Gun",
        "HJ": "Hijack",
        "CO": "Cutoff", 
        "BTN": "Button",
        "SB": "Small Blind",
        "BB": "Big Blind"
    }
    
    # Training modes
    TRAINING_MODES = {
        "open_raise": "Open Raise",
        "3_bet": "3-Bet",
        "4_bet": "4-Bet"
    }
    
    @classmethod
    def get_position_display_name(cls, position_key: str) -> str:
        """Get display name for a position key."""
        return cls.POSITION_OPTIONS.get(position_key, position_key)
    
    @classmethod
    def get_formatted_position_name(cls, position: str) -> str:
        """Format position name for display."""
        return position.replace('_', ' ').title()
