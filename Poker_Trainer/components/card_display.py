# Card display component
# Functions for displaying poker cards
class CardDisplay:
    def __init__(self):
        self.suit_symbols = {
            's': '♠',  # spades
            'h': '♥',  # hearts
            'd': '♦',  # diamonds
            'c': '♣'   # clubs
        }
        
        self.suit_colors = {
            's': '#000000',  # black
            'h': '#FF0000',  # red
            'd': '#FF0000',  # red
            'c': '#000000'   # black
        }
    
    def render_hand(self, hand: str) -> str:
        """Render a poker hand with HTML/CSS"""
        if len(hand) == 2:  # Pocket pair like "AA"
            rank = hand[0]
            return f"""
            <div style="text-align: center; margin: 20px 0;">
                <div style="display: inline-block; margin: 0 10px;">
                    <div style="background: white; border: 2px solid #333; border-radius: 10px; 
                               padding: 20px; font-size: 2em; font-weight: bold; color: #333;
                               width: 80px; height: 100px; display: flex; align-items: center; 
                               justify-content: center;">
                        {rank}
                    </div>
                </div>
                <div style="display: inline-block; margin: 0 10px;">
                    <div style="background: white; border: 2px solid #333; border-radius: 10px; 
                               padding: 20px; font-size: 2em; font-weight: bold; color: #333;
                               width: 80px; height: 100px; display: flex; align-items: center; 
                               justify-content: center;">
                        {rank}
                    </div>
                </div>
            </div>
            """
        
        elif len(hand) == 3:  # Suited/offsuit like "AKs" or "AKo"
            rank1, rank2, suit_type = hand[0], hand[1], hand[2]
            
            if suit_type == 's':  # suited
                suit1 = suit2 = '♠'
                color1 = color2 = self.suit_colors['s']
            else:  # offsuit
                suit1, suit2 = '♠', '♥'
                color1, color2 = self.suit_colors['s'], self.suit_colors['h']
            
            return f"""
            <div style="text-align: center; margin: 20px 0;">
                <div style="display: inline-block; margin: 0 10px;">
                    <div style="background: white; border: 2px solid #333; border-radius: 10px; 
                               padding: 15px; font-size: 1.5em; font-weight: bold; color: {color1};
                               width: 80px; height: 100px; display: flex; flex-direction: column;
                               align-items: center; justify-content: center;">
                        <div>{rank1}</div>
                        <div style="font-size: 1.2em;">{suit1}</div>
                    </div>
                </div>
                <div style="display: inline-block; margin: 0 10px;">
                    <div style="background: white; border: 2px solid #333; border-radius: 10px; 
                               padding: 15px; font-size: 1.5em; font-weight: bold; color: {color2};
                               width: 80px; height: 100px; display: flex; flex-direction: column;
                               align-items: center; justify-content: center;">
                        <div>{rank2}</div>
                        <div style="font-size: 1.2em;">{suit2}</div>
                    </div>
                </div>
            </div>
            <div style="text-align: center; margin-top: 10px;">
                <span style="background: #FFD700; color: #0F4C3A; padding: 5px 15px; 
                           border-radius: 20px; font-weight: bold;">
                    {hand}
                </span>
            </div>
            """
        
        return f"<div style='text-align: center; font-size: 2em; margin: 20px;'>{hand}</div>"