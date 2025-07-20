"""Hand range data for different poker positions and scenarios."""

from typing import Dict, List

class HandRanges:
    """Container for poker hand range data."""
    
    CHART_1_HANDS = {
        "any_position": {
            "paired": ["A-A", "K-K", "Q-Q", "J-J", "T-T", "9-9", "8-8", "7-7"],
            "suited": ["A-K", "A-Q", "A-J"],
            "unsuited": ["A-K", "A-Q"]
        },
        "mid_late_position": {
            "paired": ["6-6"],
            "suited": ["K-Q", "J-T", "T-9", "9-8", "A-T"],
            "unsuited": ["K-Q", "J-T", "A-J"]
        },
        "late_position": {
            "paired": ["5-5", "4-4", "3-3", "2-2"],
            "suited": ["K-J", "K-T", "Q-T", "J-9", "T-8", "9-7", "8-7", 
                      "A-9", "A-8", "A-7", "A-6", "A-5", "A-4", "A-3", "A-2"],
            "unsuited": ["K-J", "K-T", "Q-T", "J-9", "T-9"]
        },
        "hand_to_fold": {
            "suited": ["K-9", "Q-9", "J-8", "T-7", "9-6", "8-6", "7-6", "6-5", 
                      "5-4", "4-3", "3-2", "7-5", "6-4", "5-3"],
            "unsuited": ["K-9", "Q-9", "J-8", "T-8", "9-8", "8-7", "7-6", "6-5", 
                         "5-4", "4-3", "3-2", "A-T", "K-8", "Q-8", "J-7", "T-7", 
                         "9-7", "8-6", "7-5", "6-4", "5-3", "4-2", "A-9", "K-7", 
                         "Q-7", "J-6", "T-6", "9-6", "8-5", "7-4", "6-3", "5-2", 
                         "A-8", "K-6", "Q-6", "J-5", "T-5", "9-5", "8-4", "7-3", 
                         "6-2", "A-7", "K-5", "Q-5", "J-4", "T-4", "9-4", "8-3", 
                         "7-2", "A-6", "K-4", "Q-4", "J-3", "T-3", "9-3", "8-2", 
                         "A-5", "K-3", "Q-3", "J-2", "T-2", "9-2", "A-4", "K-2", 
                         "Q-2", "A-3", "A-2"]
        }
    }
    
    @classmethod
    def get_chart_1_hands(cls) -> Dict:
        """Get Chart 1 hand ranges."""
        return cls.CHART_1_HANDS
    
    @classmethod
    def get_playable_hands_for_position(cls, position: str) -> Dict:
        """Get playable hands for a specific position."""
        chart = cls.CHART_1_HANDS
        if position in chart and position != "hand_to_fold":
            return chart[position]
        return {}
    
    @classmethod
    def get_hands_to_fold(cls) -> Dict:
        """Get hands that should be folded."""
        return cls.CHART_1_HANDS.get("hand_to_fold", {})
