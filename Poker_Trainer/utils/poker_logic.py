# Poker logic utilities
# Core poker game logic and calculations
import random
from typing import Dict, List, Any

class PokerLogic:
    def __init__(self):
        self.gto_ranges = self.load_gto_ranges()
    
    def load_gto_ranges(self) -> Dict:
        """Load GTO ranges for different positions and scenarios"""
        return {
            'UTG': {
                'opening': {
                    'raise': ['AA', 'KK', 'QQ', 'JJ', 'TT', '99', 'AKs', 'AKo', 'AQs', 'AQo', 'AJs', 'KQs'],
                    'fold': ['88', '77', '66', '55', '44', '33', '22', 'AJo', 'ATs', 'A9s', 'KQo', 'KJs']
                },
                '3bet': {
                    'call': ['QQ', 'JJ', 'TT', 'AQs', 'AKs'],
                    'fold': ['99', '88', '77', 'AJo', 'ATs', 'KQs'],
                    '4bet': ['AA', 'KK', 'AKo']
                }
            },
            'BTN': {
                'opening': {
                    'raise': ['AA', 'KK', 'QQ', 'JJ', 'TT', '99', '88', '77', '66', '55', '44', '33', '22', 
                             'AKs', 'AKo', 'AQs', 'AQo', 'AJs', 'AJo', 'ATs', 'A9s', 'A8s', 'A7s', 'A6s', 'A5s', 'A4s', 'A3s', 'A2s',
                             'KQs', 'KQo', 'KJs', 'KJo', 'KTs', 'K9s', 'QJs', 'QJo', 'QTs', 'Q9s', 'JTs', 'J9s', 'T9s', 'T8s', '98s', '87s', '76s', '65s', '54s'],
                    'fold': ['T8o', '97o', '86o', '75o', '64o', '53o', '42o', 'K9o', 'Q9o', 'J9o', 'T9o']
                }
            },
            'SB': {
                'opening': {
                    'raise': ['AA', 'KK', 'QQ', 'JJ', 'TT', '99', '88', '77', '66', '55', '44', '33', '22',
                             'AKs', 'AKo', 'AQs', 'AQo', 'AJs', 'AJo', 'ATs', 'A9s', 'A8s', 'A7s', 'A6s', 'A5s', 'A4s', 'A3s', 'A2s',
                             'KQs', 'KQo', 'KJs', 'KJo', 'KTs', 'K9s', 'K8s', 'K7s', 'K6s', 'K5s', 'K4s', 'K3s', 'K2s'],
                    'fold': ['K9o', 'K8o', 'K7o', 'K6o', 'K5o', 'K4o', 'K3o', 'K2o', 'Q9o', 'J9o', 'T9o', '98o', '87o', '76o', '65o', '54o']
                }
            }
        }
    
    def get_correct_action(self, scenario: Dict[str, Any]) -> str:
        """Get the correct GTO action for a given scenario"""
        hand = scenario['hand']
        position = scenario['position']
        scenario_type = scenario['scenario_type']
        
        try:
            position_ranges = self.gto_ranges.get(position, {})
            scenario_ranges = position_ranges.get(scenario_type, {})
            
            # Check each possible action
            for action, hands in scenario_ranges.items():
                if hand in hands:
                    return action
            
            # Default to fold if hand not found in any range
            return 'fold'
            
        except Exception as e:
            print(f"Error in get_correct_action: {e}")
            return 'fold'
    
    def calculate_hand_equity(self, hand: str, position: str) -> float:
        """Calculate approximate hand equity"""
        hand_values = {
            'AA': 0.85, 'KK': 0.82, 'QQ': 0.80, 'JJ': 0.77, 'TT': 0.75,
            'AKs': 0.67, 'AKo': 0.65, 'AQs': 0.63, 'AQo': 0.60, 'AJs': 0.58,
            'AJo': 0.55, 'ATs': 0.56, 'ATo': 0.53, 'KQs': 0.60, 'KQo': 0.57
        }
        
        base_equity = hand_values.get(hand, 0.40)  # Default equity
        
        # Position adjustment
# Position adjustment
        position_multiplier = {
            'UTG': 0.90,
            'MP': 0.95,
            'CO': 1.00,
            'BTN': 1.05,
            'SB': 0.85,
            'BB': 0.80
        }
        
        multiplier = position_multiplier.get(position, 1.0)
        return min(base_equity * multiplier, 1.0)
    
    def get_hand_category(self, hand: str) -> str:
        """Categorize poker hands"""
        if len(hand) == 2 and hand[0] == hand[1]:
            return "pocket_pair"
        elif len(hand) == 3:
            if hand[2] == 's':
                return "suited"
            else:
                return "offsuit"
        return "unknown"
    
    def should_3bet(self, hand: str, position: str, opponent_position: str) -> bool:
        """Determine if hand should 3-bet based on GTO"""
        three_bet_hands = {
            'vs_UTG': ['AA', 'KK', 'QQ', 'JJ', 'AKs', 'AKo', 'A5s', 'A4s'],
            'vs_BTN': ['AA', 'KK', 'QQ', 'JJ', 'TT', '99', '88', '77', '66', '55', '44', '33', '22',
                      'AKs', 'AKo', 'AQs', 'AQo', 'AJs', 'AJo', 'ATs', 'A9s', 'A8s', 'A7s', 'A6s', 'A5s', 'A4s', 'A3s', 'A2s',
                      'KQs', 'KJs', 'KTs', 'K9s', 'QJs', 'QTs', 'JTs', 'T9s', '98s', '87s', '76s', '65s', '54s']
        }
        
        range_key = f'vs_{opponent_position}'
        return hand in three_bet_hands.get(range_key, [])