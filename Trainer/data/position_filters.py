"""Position filtering logic for hand ranges."""

from typing import List, Tuple
from .hand_ranges import HandRanges

class PositionFilter:
    """Filter hands based on position requirements."""
    
    @staticmethod
    def get_all_hands() -> List[Tuple[str, str, bool]]:
        """Get all possible hands with their correct action (True=Play, False=Fold)."""
        hands = []
        chart_1_hands = HandRanges.get_chart_1_hands()
        
        # From chart_1 - position-based hands (these should be played)
        for position, categories in chart_1_hands.items():
            if position != "hand_to_fold":
                for category, hand_list in categories.items():
                    for hand in hand_list:
                        if category == "suited":
                            hands.append((f"{hand}s", position, True))
                        elif category == "unsuited":
                            hands.append((f"{hand}o", position, True))
                        else:  # paired
                            hands.append((hand, position, True))
        
        # From chart_1 - hands to fold
        fold_hands = HandRanges.get_hands_to_fold()
        for category, hand_list in fold_hands.items():
            for hand in hand_list:
                if category == "suited":
                    hands.append((f"{hand}s", "fold", False))
                else:  # unsuited
                    hands.append((f"{hand}o", "fold", False))
        
        return hands
    
    @staticmethod
    def get_hands_for_position(position_filter: str) -> List[Tuple[str, str, bool]]:
        """Get hands filtered by position."""
        all_hands = PositionFilter.get_all_hands()
        
        if position_filter == "all":
            return all_hands
        elif position_filter == "any_position":
            return [h for h in all_hands if h[1] == "any_position" or h[1] == "fold"]
        elif position_filter == "mid_late_position":
            return [h for h in all_hands 
                   if h[1] in ["any_position", "mid_late_position"] or h[1] == "fold"]
        elif position_filter == "late_position":
            return [h for h in all_hands 
                   if h[1] in ["any_position", "mid_late_position", "late_position"] 
                   or h[1] == "fold"]
        
        return all_hands
