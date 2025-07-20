"""Chart 2 hand data for alternative training scenarios."""

from typing import Dict, List

class ChartData:
    """Container for Chart 2 hand data."""
    
    CHART_2_HANDS = {
        "playable": [
            "A-A", "K-K", "Q-Q", "J-J", "T-T", "9-9", "8-8", "7-7", "6-6", 
            "5-5", "4-4", "3-3", "2-2", "A-K", "A-Q", "A-J", "A-T", "A-9", 
            "A-8", "A-7", "A-6", "A-5", "A-4", "A-3", "A-2", "K-Q", "K-J", 
            "K-T", "Q-J", "Q-T", "J-T"
        ],
        "playable_to_an_extent": [
            "K-9", "Q-9", "J-9", "T-9", "9-8", "8-7", "7-6", "6-5", "5-4"
        ],
        "playable_until_first_raise": [
            "K-8", "K-7", "Q-8", "J-8", "T-8", "9-7", "8-6", "7-5", 
            "6-4", "5-3", "4-3", "3-2"
        ],
        "unplayable": [
            "K-6", "K-5", "K-4", "K-3", "K-2", "Q-7", "Q-6", "Q-5", 
            "Q-4", "Q-3", "Q-2", "J-7", "J-6", "J-5", "J-4", "J-3", 
            "J-2", "T-7", "T-6", "T-5", "T-4", "T-3", "T-2", "9-6", 
            "9-5", "9-4", "9-3", "9-2", "8-5", "8-4", "8-3", "8-2", 
            "7-4", "7-3", "7-2", "6-3", "6-2", "5-2", "4-2"
        ]
    }
    
    @classmethod
    def get_chart_2_hands(cls) -> Dict[str, List[str]]:
        """Get Chart 2 hand ranges."""
        return cls.CHART_2_HANDS
    
    @classmethod
    def get_playable_hands(cls) -> List[str]:
        """Get all playable hands from Chart 2."""
        return cls.CHART_2_HANDS.get("playable", [])
    
    @classmethod
    def get_marginal_hands(cls) -> List[str]:
        """Get marginal playable hands."""
        return cls.CHART_2_HANDS.get("playable_to_an_extent", [])
    
    @classmethod
    def get_conditional_hands(cls) -> List[str]:
        """Get hands playable until first raise."""
        return cls.CHART_2_HANDS.get("playable_until_first_raise", [])
    
    @classmethod
    def get_unplayable_hands(cls) -> List[str]:
        """Get unplayable hands."""
        return cls.CHART_2_HANDS.get("unplayable", [])
