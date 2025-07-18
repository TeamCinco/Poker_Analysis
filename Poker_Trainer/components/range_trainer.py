# Range trainer component
# Core training logic
import random
import json
import os
from typing import Dict, List, Any

class RangeTrainer:
    def __init__(self):
        self.ranges = self.load_ranges()
        self.hands = self.generate_all_hands()
    
    def load_ranges(self) -> Dict:
        """Load preflop ranges from JSON files"""
        ranges = {}
        range_files = [
            'preflop_ranges.json',
            '3bet_ranges.json',
            '4bet_ranges.json'
        ]
        
        for file in range_files:
            try:
                with open(f'data/ranges/{file}', 'r') as f:
                    ranges[file.replace('.json', '')] = json.load(f)
            except FileNotFoundError:
                # Create default ranges if files don't exist
                ranges[file.replace('.json', '')] = self.create_default_ranges()
        
        return ranges
    
    def create_default_ranges(self) -> Dict:
        """Create default GTO-based ranges"""
        return {
            'UTG': {
                'raise': ['AA', 'KK', 'QQ', 'JJ', 'TT', '99', 'AKs', 'AKo', 'AQs', 'AQo', 'AJs', 'AJo', 'ATs', 'KQs', 'KQo', 'KJs'],
                'fold': ['22', '33', '44', '55', '66', '77', '88', 'A9s', 'A8s', 'A7s', 'A6s', 'A5s', 'A4s', 'A3s', 'A2s']
            },
            'BTN': {
                'raise': ['AA', 'KK', 'QQ', 'JJ', 'TT', '99', '88', '77', '66', '55', '44', '33', '22', 'AKs', 'AKo', 'AQs', 'AQo', 'AJs', 'AJo', 'ATs', 'A9s', 'A8s', 'A7s', 'A6s', 'A5s', 'A4s', 'A3s', 'A2s'],
                'fold': ['T8o', '97o', '86o', '75o', '64o', '53o', '42o']
            }
        }
    
    def generate_all_hands(self) -> List[str]:
        """Generate all possible poker hands"""
        ranks = ['A', 'K', 'Q', 'J', 'T', '9', '8', '7', '6', '5', '4', '3', '2']
        hands = []
        
        # Pocket pairs
        for rank in ranks:
            hands.append(f"{rank}{rank}")
        
        # Suited hands
        for i, rank1 in enumerate(ranks):
            for rank2 in ranks[i+1:]:
                hands.append(f"{rank1}{rank2}s")
        
        # Offsuit hands
        for i, rank1 in enumerate(ranks):
            for rank2 in ranks[i+1:]:
                hands.append(f"{rank1}{rank2}o")
        
        return hands
    
    def generate_scenario(self, training_mode: str, position: str, stack_depth: int) -> Dict[str, Any]:
        """Generate a training scenario"""
        hand = random.choice(self.hands)
        
        if position == "Random":
            position = random.choice(['UTG', 'MP', 'CO', 'BTN', 'SB', 'BB'])
        
        scenario = {
            'hand': hand,
            'position': position,
            'stack_depth': stack_depth,
            'scenario_type': training_mode.lower().replace(' ', '_'),
            'timestamp': None
        }
        
        # Add opponent action for 3-bet/4-bet scenarios
        if '3-bet' in training_mode.lower():
            scenario['opponent_action'] = 'raised'
        elif '4-bet' in training_mode.lower():
            scenario['opponent_action'] = '3-bet'
        
        return scenario
    
    def get_hand_strength(self, hand: str) -> float:
        """Calculate hand strength (simplified)"""
        hand_strengths = {
            'AA': 1.0, 'KK': 0.95, 'QQ': 0.9, 'JJ': 0.85, 'TT': 0.8,
            'AKs': 0.75, 'AKo': 0.7, 'AQs': 0.65, 'AQo': 0.6
        }
        
        return hand_strengths.get(hand, 0.3)