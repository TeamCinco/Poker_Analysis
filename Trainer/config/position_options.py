"""Position configuration and options."""

from typing import Dict, List

class PositionConfig:
    """Position-based configuration for poker trainer."""
    
    # Legacy position options (for backward compatibility)
    POSITION_OPTIONS = {
        "all": "All Positions",
        "any_position": "Any Position (Premium Hands)",
        "mid_late_position": "Mid/Late Position",
        "late_position": "Late Position (Button/Cutoff)"
    }
    
    # Table size options
    TABLE_SIZE_OPTIONS = {
        "6-max": "6-Max Tables",
        "9-max": "9-Max Tables"
    }
    
    # Specific position options for each table size
    SPECIFIC_POSITIONS = {
        "6-max": {
            "UTG": "Under The Gun",
            "HJ": "Hijack", 
            "CO": "Cutoff",
            "BTN": "Button",
            "SB": "Small Blind",
            "BB": "Big Blind"
        },
        "9-max": {
            "UTG": "Under The Gun",
            "UTG1": "Under The Gun +1",
            "MP1": "Middle Position 1",
            "MP2": "Middle Position 2", 
            "HJ": "Hijack",
            "CO": "Cutoff",
            "BTN": "Button",
            "SB": "Small Blind",
            "BB": "Big Blind"
        }
    }
    
    # Position abbreviations for display (combined from both table sizes)
    POSITION_DISPLAY = {
        "UTG": "Under The Gun",
        "UTG1": "Under The Gun +1",
        "MP1": "Middle Position 1",
        "MP2": "Middle Position 2",
        "HJ": "Hijack",
        "CO": "Cutoff", 
        "BTN": "Button",
        "SB": "Small Blind",
        "BB": "Big Blind"
    }
    
    # Action options
    ACTION_OPTIONS = {
        "Opening-Raise": "Opening Raise",
        "3-Bet": "3-Bet",
        "4-Bet": "4-Bet"
    }
    
    # Training modes (legacy)
    TRAINING_MODES = {
        "open_raise": "Open Raise",
        "3_bet": "3-Bet",
        "4_bet": "4-Bet"
    }
    
    # Training scope options
    TRAINING_SCOPE_OPTIONS = {
        "single_scenario": "Single Position/Action",
        "position_all_actions": "Single Position - All Actions",
        "action_all_positions": "Single Action - All Positions",
        "table_size_all": "All Positions & Actions for Table Size"
    }
    
    @classmethod
    def get_position_display_name(cls, position_key: str) -> str:
        """Get display name for a position key."""
        return cls.POSITION_OPTIONS.get(position_key, position_key)
    
    @classmethod
    def get_formatted_position_name(cls, position: str) -> str:
        """Format position name for display."""
        return position.replace('_', ' ').title()
    
    @classmethod
    def get_positions_for_table_size(cls, table_size: str) -> List[str]:
        """Get list of positions for a specific table size."""
        return list(cls.SPECIFIC_POSITIONS.get(table_size, {}).keys())
    
    @classmethod
    def get_position_full_name(cls, position: str) -> str:
        """Get full display name for a position."""
        return cls.POSITION_DISPLAY.get(position, position)
    
    @classmethod
    def get_available_table_sizes(cls) -> List[str]:
        """Get available table sizes."""
        return list(cls.TABLE_SIZE_OPTIONS.keys())
    
    @classmethod
    def get_available_actions(cls) -> List[str]:
        """Get available actions."""
        return list(cls.ACTION_OPTIONS.keys())
    
    @classmethod
    def format_scenario_display(cls, table_size: str, position: str, action: str) -> str:
        """Format a complete scenario for display."""
        position_name = cls.get_position_full_name(position)
        action_name = cls.ACTION_OPTIONS.get(action, action)
        return f"{position_name} {action_name} ({table_size})"
