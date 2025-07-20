"""Application configuration and settings."""

import streamlit as st

class AppConfig:
    """Application configuration settings."""
    
    PAGE_TITLE = "Poker Hand Trainer"
    PAGE_ICON = "♠️"
    LAYOUT = "wide"
    
    # UI Settings
    CARD_FONT_SIZE = "3em"
    CARD_PADDING = "20px"
    CARD_BORDER_RADIUS = "15px"
    CARD_MIN_WIDTH = "80px"
    CARD_GAP = "20px"
    CARD_MARGIN = "20px 0"
    
    # Colors
    CARD_BACKGROUND = "white"
    CARD_BORDER = "3px solid #333"
    CARD_SHADOW = "0 4px 8px rgba(0,0,0,0.3)"
    SUITED_COLOR = "green"
    RED_COLOR = "red"
    BLACK_COLOR = "black"
    OFFSUIT_COLOR = "#666"
    PAIR_COLOR = "#9f6000"
    
    # Suits
    SPADE_SUIT = "♠️"
    HEART_SUIT = "♥️"
    
    # Session State Keys
    SELECTED_POSITION = 'selected_position'
    HANDS = 'hands'
    CURRENT_HAND_INDEX = 'current_hand_index'
    SCORE = 'score'
    TOTAL_ANSWERED = 'total_answered'
    SHOW_RESULT = 'show_result'
    LAST_ANSWER = 'last_answer'

def configure_page():
    """Configure Streamlit page settings."""
    st.set_page_config(
        page_title=AppConfig.PAGE_TITLE,
        page_icon=AppConfig.PAGE_ICON,
        layout=AppConfig.LAYOUT
    )
