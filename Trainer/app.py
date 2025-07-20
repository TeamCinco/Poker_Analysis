import streamlit as st
import random
from typing import Dict, List, Tuple

# Card display functions
def get_card_symbol(rank: str) -> str:
    """Convert rank to display symbol"""
    symbols = {
        'A': 'A', 'K': 'K', 'Q': 'Q', 'J': 'J', 'T': '10',
        '9': '9', '8': '8', '7': '7', '6': '6', '5': '5',
        '4': '4', '3': '3', '2': '2'
    }
    return symbols.get(rank, rank)

def display_cards(hand: str) -> str:
    """Display cards with visual styling"""
    # Parse the hand (e.g., "A-Ks", "7-7", "K-Qo")
    if hand.endswith('s'):
        # Suited hand
        cards = hand[:-1].split('-')
        card1 = get_card_symbol(cards[0])
        card2 = get_card_symbol(cards[1])
        suit = "♠️"  # Using spade as example for suited
        return f"""
        <div style="display: flex; justify-content: center; gap: 20px; margin: 20px 0;">
            <div style="background: white; border: 3px solid #333; border-radius: 15px; 
                        padding: 20px; font-size: 3em; font-weight: bold; color: black;
                        box-shadow: 0 4px 8px rgba(0,0,0,0.3); min-width: 80px; text-align: center;">
                {card1}<br><span style="font-size: 0.6em; color: green;">{suit}</span>
            </div>
            <div style="background: white; border: 3px solid #333; border-radius: 15px; 
                        padding: 20px; font-size: 3em; font-weight: bold; color: black;
                        box-shadow: 0 4px 8px rgba(0,0,0,0.3); min-width: 80px; text-align: center;">
                {card2}<br><span style="font-size: 0.6em; color: green;">{suit}</span>
            </div>
        </div>
        <p style="text-align: center; color: green; font-weight: bold; font-size: 1.2em;">SUITED</p>
        """
    elif hand.endswith('o'):
        # Offsuit hand
        cards = hand[:-1].split('-')
        card1 = get_card_symbol(cards[0])
        card2 = get_card_symbol(cards[1])
        suit1 = "♠️"
        suit2 = "♥️"
        return f"""
        <div style="display: flex; justify-content: center; gap: 20px; margin: 20px 0;">
            <div style="background: white; border: 3px solid #333; border-radius: 15px; 
                        padding: 20px; font-size: 3em; font-weight: bold; color: black;
                        box-shadow: 0 4px 8px rgba(0,0,0,0.3); min-width: 80px; text-align: center;">
                {card1}<br><span style="font-size: 0.6em;">{suit1}</span>
            </div>
            <div style="background: white; border: 3px solid #333; border-radius: 15px; 
                        padding: 20px; font-size: 3em; font-weight: bold; color: red;
                        box-shadow: 0 4px 8px rgba(0,0,0,0.3); min-width: 80px; text-align: center;">
                {card2}<br><span style="font-size: 0.6em; color: red;">{suit2}</span>
            </div>
        </div>
        <p style="text-align: center; color: #666; font-weight: bold; font-size: 1.2em;">OFFSUIT</p>
        """
    else:
        # Pocket pair
        cards = hand.split('-')
        card = get_card_symbol(cards[0])
        return f"""
        <div style="display: flex; justify-content: center; gap: 20px; margin: 20px 0;">
            <div style="background: white; border: 3px solid #333; border-radius: 15px; 
                        padding: 20px; font-size: 3em; font-weight: bold; color: red;
                        box-shadow: 0 4px 8px rgba(0,0,0,0.3); min-width: 80px; text-align: center;">
                {card}<br><span style="font-size: 0.6em; color: red;">♥️</span>
            </div>
            <div style="background: white; border: 3px solid #333; border-radius: 15px; 
                        padding: 20px; font-size: 3em; font-weight: bold; color: black;
                        box-shadow: 0 4px 8px rgba(0,0,0,0.3); min-width: 80px; text-align: center;">
                {card}<br><span style="font-size: 0.6em;">♠️</span>
            </div>
        </div>
        <p style="text-align: center; color: #9f6000; font-weight: bold; font-size: 1.2em;">POCKET PAIR</p>
        """

# Hand ranges data
chart_1_hands = {
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
        "suited": ["K-J", "K-T", "Q-T", "J-9", "T-8", "9-7", "8-7", "A-9", "A-8", "A-7", "A-6", "A-5", "A-4", "A-3", "A-2"],
        "unsuited": ["K-J", "K-T", "Q-T", "J-9", "T-9"]
    },
    "hand_to_fold": {
        "suited": ["K-9", "Q-9", "J-8", "T-7", "9-6", "8-6", "7-6", "6-5", "5-4", "4-3", "3-2", "7-5", "6-4", "5-3"],
        "unsuited": ["K-9", "Q-9", "J-8", "T-8", "9-8", "8-7", "7-6", "6-5", "5-4", "4-3", "3-2", "A-T", "K-8", "Q-8", "J-7", "T-7", "9-7", "8-6", "7-5", "6-4", "5-3", "4-2", "A-9", "K-7", "Q-7", "J-6", "T-6", "9-6", "8-5", "7-4", "6-3", "5-2", "A-8", "K-6", "Q-6", "J-5", "T-5", "9-5", "8-4", "7-3", "6-2", "A-7", "K-5", "Q-5", "J-4", "T-4", "9-4", "8-3", "7-2", "A-6", "K-4", "Q-4", "J-3", "T-3", "9-3", "8-2", "A-5", "K-3", "Q-3", "J-2", "T-2", "9-2", "A-4", "K-2", "Q-2", "A-3", "A-2"]
    }
}

chart_2_hands = {
    "playable": ["A-A", "K-K", "Q-Q", "J-J", "T-T", "9-9", "8-8", "7-7", "6-6", "5-5", "4-4", "3-3", "2-2", "A-K", "A-Q", "A-J", "A-T", "A-9", "A-8", "A-7", "A-6", "A-5", "A-4", "A-3", "A-2", "K-Q", "K-J", "K-T", "Q-J", "Q-T", "J-T"],
    "playable_to_an_extent": ["K-9", "Q-9", "J-9", "T-9", "9-8", "8-7", "7-6", "6-5", "5-4"],
    "playable_until_first_raise": ["K-8", "K-7", "Q-8", "J-8", "T-8", "9-7", "8-6", "7-5", "6-4", "5-3", "4-3", "3-2"],
    "unplayable": ["K-6", "K-5", "K-4", "K-3", "K-2", "Q-7", "Q-6", "Q-5", "Q-4", "Q-3", "Q-2", "J-7", "J-6", "J-5", "J-4", "J-3", "J-2", "T-7", "T-6", "T-5", "T-4", "T-3", "T-2", "9-6", "9-5", "9-4", "9-3", "9-2", "8-5", "8-4", "8-3", "8-2", "7-4", "7-3", "7-2", "6-3", "6-2", "5-2", "4-2"]
}

def get_all_hands() -> List[Tuple[str, str, bool]]:
    """Get all possible hands with their correct action (True=Play, False=Fold)"""
    hands = []
    
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
    for category, hand_list in chart_1_hands["hand_to_fold"].items():
        for hand in hand_list:
            if category == "suited":
                hands.append((f"{hand}s", "fold", False))
            else:  # unsuited
                hands.append((f"{hand}o", "fold", False))
    
    return hands

def get_hands_for_position(position_filter: str) -> List[Tuple[str, str, bool]]:
    """Get hands filtered by position"""
    all_hands = get_all_hands()
    if position_filter == "all":
        return all_hands
    elif position_filter == "any_position":
        return [h for h in all_hands if h[1] == "any_position" or h[1] == "fold"]
    elif position_filter == "mid_late_position":
        return [h for h in all_hands if h[1] in ["any_position", "mid_late_position"] or h[1] == "fold"]
    elif position_filter == "late_position":
        return [h for h in all_hands if h[1] in ["any_position", "mid_late_position", "late_position"] or h[1] == "fold"]
    return all_hands

def initialize_session_state():
    """Initialize session state variables"""
    if 'selected_position' not in st.session_state:
        st.session_state.selected_position = "all"
    
    if 'hands' not in st.session_state:
        st.session_state.hands = get_hands_for_position(st.session_state.selected_position)
        random.shuffle(st.session_state.hands)
    
    if 'current_hand_index' not in st.session_state:
        st.session_state.current_hand_index = 0
    
    if 'score' not in st.session_state:
        st.session_state.score = 0
    
    if 'total_answered' not in st.session_state:
        st.session_state.total_answered = 0
    
    if 'show_result' not in st.session_state:
        st.session_state.show_result = False
    
    if 'last_answer' not in st.session_state:
        st.session_state.last_answer = None

def get_current_hand():
    """Get the current hand to display"""
    if st.session_state.current_hand_index >= len(st.session_state.hands):
        # Reshuffle when we've gone through all hands
        random.shuffle(st.session_state.hands)
        st.session_state.current_hand_index = 0
    
    return st.session_state.hands[st.session_state.current_hand_index]

def answer_question(user_answer: bool):
    """Process the user's answer"""
    current_hand, position, correct_answer = get_current_hand()
    
    st.session_state.last_answer = {
        'hand': current_hand,
        'position': position,
        'user_answer': user_answer,
        'correct_answer': correct_answer,
        'correct': user_answer == correct_answer
    }
    
    if user_answer == correct_answer:
        st.session_state.score += 1
    
    st.session_state.total_answered += 1
    st.session_state.show_result = True

def next_hand():
    """Move to the next hand"""
    st.session_state.current_hand_index += 1
    st.session_state.show_result = False
    st.session_state.last_answer = None

def reset_training():
    """Reset the training session"""
    st.session_state.score = 0
    st.session_state.total_answered = 0
    st.session_state.current_hand_index = 0
    st.session_state.show_result = False
    st.session_state.last_answer = None
    random.shuffle(st.session_state.hands)

# Streamlit app
def main():
    st.set_page_config(
        page_title="Poker Hand Trainer",
        page_icon="♠️",
        layout="wide"
    )
    
    st.title("♠️ Poker Hand Trainer ♠️")
    
    initialize_session_state()
    
    # Position selector
    st.subheader("🎯 Select Training Position:")
    position_options = {
        "all": "All Positions",
        "any_position": "Any Position (Premium Hands)",
        "mid_late_position": "Mid/Late Position",
        "late_position": "Late Position (Button/Cutoff)"
    }
    
    selected_position = st.selectbox(
        "Choose which position to train:",
        options=list(position_options.keys()),
        format_func=lambda x: position_options[x],
        index=list(position_options.keys()).index(st.session_state.selected_position)
    )
    
    # Update hands if position changed
    if selected_position != st.session_state.selected_position:
        st.session_state.selected_position = selected_position
        st.session_state.hands = get_hands_for_position(selected_position)
        random.shuffle(st.session_state.hands)
        st.session_state.current_hand_index = 0
        st.session_state.show_result = False
        st.session_state.last_answer = None
        st.rerun()
    
    # Stats
    if st.session_state.total_answered > 0:
        accuracy = (st.session_state.score / st.session_state.total_answered) * 100
        st.metric("Accuracy", f"{accuracy:.1f}%", 
                 f"{st.session_state.score}/{st.session_state.total_answered}")
    
    st.divider()
    
    # Main training area
    if not st.session_state.show_result:
        current_hand, position, correct_answer = get_current_hand()
        
        st.subheader("What should you do with this hand?")
        
        # Display the visual cards
        st.markdown(display_cards(current_hand), unsafe_allow_html=True)
        
        # Action buttons
        col1, col2, col3 = st.columns([1, 1, 1])
        
        with col1:
            if st.button("🔴 FOLD", key="fold", use_container_width=True, 
                        help="Click if you should fold this hand"):
                answer_question(False)
        
        with col3:
            if st.button("🟢 PLAY", key="play", use_container_width=True,
                        help="Click if you should play/raise this hand"):
                answer_question(True)
    
    else:
        # Show result
        result = st.session_state.last_answer
        
        # Display the visual cards
        st.markdown(display_cards(result['hand']), unsafe_allow_html=True)
        
        if result['correct']:
            st.success(f"✅ Correct! {result['hand']} should be {'PLAYED' if result['correct_answer'] else 'FOLDED'}")
        else:
            st.error(f"❌ Wrong! {result['hand']} should be {'PLAYED' if result['correct_answer'] else 'FOLDED'}")
        
        # Show position info if it's a playable hand
        if result['position'] != "fold":
            st.info(f"📍 Position: {result['position'].replace('_', ' ').title()}")
        
        if st.button("Next Hand ➡️", key="next", use_container_width=True):
            next_hand()
            st.rerun()
    
    # Reset button
    st.divider()
    if st.button("🔄 Reset Training", use_container_width=True):
        reset_training()
        st.rerun()

if __name__ == "__main__":
    main()
