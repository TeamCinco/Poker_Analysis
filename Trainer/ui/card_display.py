"""Card display and visualization components."""

from config.settings import AppConfig

class CardDisplay:
    """Handle card visualization and display."""
    
    @staticmethod
    def get_card_symbol(rank: str) -> str:
        """Convert rank to display symbol."""
        symbols = {
            'A': 'A', 'K': 'K', 'Q': 'Q', 'J': 'J', 'T': '10',
            '9': '9', '8': '8', '7': '7', '6': '6', '5': '5',
            '4': '4', '3': '3', '2': '2'
        }
        return symbols.get(rank, rank)
    
    @staticmethod
    def _get_card_base_style() -> str:
        """Get base CSS style for cards."""
        return f"""
            background: {AppConfig.CARD_BACKGROUND}; 
            border: {AppConfig.CARD_BORDER}; 
            border-radius: {AppConfig.CARD_BORDER_RADIUS};
            padding: {AppConfig.CARD_PADDING}; 
            font-size: {AppConfig.CARD_FONT_SIZE}; 
            font-weight: bold;
            box-shadow: {AppConfig.CARD_SHADOW}; 
            min-width: {AppConfig.CARD_MIN_WIDTH}; 
            text-align: center;
        """
    
    @staticmethod
    def _get_container_style() -> str:
        """Get container style for card display."""
        return f"""
            display: flex; 
            justify-content: center; 
            gap: {AppConfig.CARD_GAP}; 
            margin: {AppConfig.CARD_MARGIN};
        """
    
    @classmethod
    def display_suited_cards(cls, card1: str, card2: str) -> str:
        """Display suited cards."""
        suit = AppConfig.SPADE_SUIT
        base_style = cls._get_card_base_style()
        container_style = cls._get_container_style()
        
        return f"""
        <div style="{container_style}">
            <div style="{base_style} color: {AppConfig.BLACK_COLOR};">
                {card1}<br><span style="font-size: 0.6em; color: {AppConfig.SUITED_COLOR};">{suit}</span>
            </div>
            <div style="{base_style} color: {AppConfig.BLACK_COLOR};">
                {card2}<br><span style="font-size: 0.6em; color: {AppConfig.SUITED_COLOR};">{suit}</span>
            </div>
        </div>
        <p style="text-align: center; color: {AppConfig.SUITED_COLOR}; font-weight: bold; font-size: 1.2em;">SUITED</p>
        """
    
    @classmethod
    def display_offsuit_cards(cls, card1: str, card2: str) -> str:
        """Display offsuit cards."""
        suit1 = AppConfig.SPADE_SUIT
        suit2 = AppConfig.HEART_SUIT
        base_style = cls._get_card_base_style()
        container_style = cls._get_container_style()
        
        return f"""
        <div style="{container_style}">
            <div style="{base_style} color: {AppConfig.BLACK_COLOR};">
                {card1}<br><span style="font-size: 0.6em;">{suit1}</span>
            </div>
            <div style="{base_style} color: {AppConfig.RED_COLOR};">
                {card2}<br><span style="font-size: 0.6em; color: {AppConfig.RED_COLOR};">{suit2}</span>
            </div>
        </div>
        <p style="text-align: center; color: {AppConfig.OFFSUIT_COLOR}; font-weight: bold; font-size: 1.2em;">OFFSUIT</p>
        """
    
    @classmethod
    def display_pocket_pair(cls, card: str) -> str:
        """Display pocket pair."""
        base_style = cls._get_card_base_style()
        container_style = cls._get_container_style()
        
        return f"""
        <div style="{container_style}">
            <div style="{base_style} color: {AppConfig.RED_COLOR};">
                {card}<br><span style="font-size: 0.6em; color: {AppConfig.RED_COLOR};">{AppConfig.HEART_SUIT}</span>
            </div>
            <div style="{base_style} color: {AppConfig.BLACK_COLOR};">
                {card}<br><span style="font-size: 0.6em;">{AppConfig.SPADE_SUIT}</span>
            </div>
        </div>
        <p style="text-align: center; color: {AppConfig.PAIR_COLOR}; font-weight: bold; font-size: 1.2em;">POCKET PAIR</p>
        """
    
    @classmethod
    def display_cards(cls, hand: str) -> str:
        """Display cards with visual styling based on hand type."""
        if hand.endswith('s'):
            # Suited hand (e.g., "AKs")
            hand_base = hand[:-1]  # Remove 's'
            if len(hand_base) == 2:
                card1 = cls.get_card_symbol(hand_base[0])
                card2 = cls.get_card_symbol(hand_base[1])
            else:
                # Handle hands like "T9s" where T = 10
                card1 = cls.get_card_symbol(hand_base[0])
                card2 = cls.get_card_symbol(hand_base[1])
            return cls.display_suited_cards(card1, card2)
        elif hand.endswith('o'):
            # Offsuit hand (e.g., "AKo")
            hand_base = hand[:-1]  # Remove 'o'
            if len(hand_base) == 2:
                card1 = cls.get_card_symbol(hand_base[0])
                card2 = cls.get_card_symbol(hand_base[1])
            else:
                card1 = cls.get_card_symbol(hand_base[0])
                card2 = cls.get_card_symbol(hand_base[1])
            return cls.display_offsuit_cards(card1, card2)
        elif len(hand) == 2 and hand[0] == hand[1]:
            # Pocket pair (e.g., "AA", "KK")
            card = cls.get_card_symbol(hand[0])
            return cls.display_pocket_pair(card)
        else:
            # Handle legacy format with dashes (backward compatibility)
            if '-' in hand:
                if hand.endswith('s'):
                    # Suited hand with dash format
                    cards = hand[:-1].split('-')
                    card1 = cls.get_card_symbol(cards[0])
                    card2 = cls.get_card_symbol(cards[1])
                    return cls.display_suited_cards(card1, card2)
                elif hand.endswith('o'):
                    # Offsuit hand with dash format
                    cards = hand[:-1].split('-')
                    card1 = cls.get_card_symbol(cards[0])
                    card2 = cls.get_card_symbol(cards[1])
                    return cls.display_offsuit_cards(card1, card2)
                else:
                    # Pocket pair with dash format
                    cards = hand.split('-')
                    card = cls.get_card_symbol(cards[0])
                    return cls.display_pocket_pair(card)
            else:
                # Fallback - treat as pocket pair
                card = cls.get_card_symbol(hand[0])
                return cls.display_pocket_pair(card)
