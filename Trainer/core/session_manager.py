"""Session state management for the Poker Trainer."""

import streamlit as st
import random
from typing import List, Tuple, Optional, Dict
from config.settings import AppConfig
from data.position_filters import PositionFilter

class SessionManager:
    """Manage Streamlit session state for the poker trainer."""
    
    @staticmethod
    def initialize_session_state():
        """Initialize all session state variables."""
        # Default position selection
        if AppConfig.SELECTED_POSITION not in st.session_state:
            st.session_state[AppConfig.SELECTED_POSITION] = "all"
        
        # Initialize hands list
        if AppConfig.HANDS not in st.session_state:
            selected_position = st.session_state[AppConfig.SELECTED_POSITION]
            st.session_state[AppConfig.HANDS] = PositionFilter.get_hands_for_position(selected_position)
            random.shuffle(st.session_state[AppConfig.HANDS])
        
        # Initialize hand tracking
        if AppConfig.CURRENT_HAND_INDEX not in st.session_state:
            st.session_state[AppConfig.CURRENT_HAND_INDEX] = 0
        
        # Initialize scoring
        if AppConfig.SCORE not in st.session_state:
            st.session_state[AppConfig.SCORE] = 0
        
        if AppConfig.TOTAL_ANSWERED not in st.session_state:
            st.session_state[AppConfig.TOTAL_ANSWERED] = 0
        
        # Initialize UI state
        if AppConfig.SHOW_RESULT not in st.session_state:
            st.session_state[AppConfig.SHOW_RESULT] = False
        
        if AppConfig.LAST_ANSWER not in st.session_state:
            st.session_state[AppConfig.LAST_ANSWER] = None
    
    @staticmethod
    def get_selected_position() -> str:
        """Get the currently selected position."""
        return st.session_state.get(AppConfig.SELECTED_POSITION, "all")
    
    @staticmethod
    def update_position(new_position: str):
        """Update the selected position and refresh hands."""
        if new_position != st.session_state[AppConfig.SELECTED_POSITION]:
            st.session_state[AppConfig.SELECTED_POSITION] = new_position
            st.session_state[AppConfig.HANDS] = PositionFilter.get_hands_for_position(new_position)
            random.shuffle(st.session_state[AppConfig.HANDS])
            st.session_state[AppConfig.CURRENT_HAND_INDEX] = 0
            st.session_state[AppConfig.SHOW_RESULT] = False
            st.session_state[AppConfig.LAST_ANSWER] = None
    
    @staticmethod
    def get_current_hand() -> Tuple[str, str, bool]:
        """Get the current hand to display."""
        hands = st.session_state[AppConfig.HANDS]
        current_index = st.session_state[AppConfig.CURRENT_HAND_INDEX]
        
        if current_index >= len(hands):
            # Reshuffle when we've gone through all hands
            random.shuffle(hands)
            st.session_state[AppConfig.CURRENT_HAND_INDEX] = 0
            current_index = 0
        
        return hands[current_index]
    
    @staticmethod
    def get_score_data() -> Tuple[int, int]:
        """Get current score and total answered."""
        score = st.session_state.get(AppConfig.SCORE, 0)
        total = st.session_state.get(AppConfig.TOTAL_ANSWERED, 0)
        return score, total
    
    @staticmethod
    def is_showing_result() -> bool:
        """Check if currently showing result."""
        return st.session_state.get(AppConfig.SHOW_RESULT, False)
    
    @staticmethod
    def get_last_answer() -> Optional[Dict]:
        """Get the last answer data."""
        return st.session_state.get(AppConfig.LAST_ANSWER)
    
    @staticmethod
    def advance_to_next_hand():
        """Move to the next hand."""
        st.session_state[AppConfig.CURRENT_HAND_INDEX] += 1
        st.session_state[AppConfig.SHOW_RESULT] = False
        st.session_state[AppConfig.LAST_ANSWER] = None
    
    @staticmethod
    def reset_training():
        """Reset the entire training session."""
        st.session_state[AppConfig.SCORE] = 0
        st.session_state[AppConfig.TOTAL_ANSWERED] = 0
        st.session_state[AppConfig.CURRENT_HAND_INDEX] = 0
        st.session_state[AppConfig.SHOW_RESULT] = False
        st.session_state[AppConfig.LAST_ANSWER] = None
        
        # Reshuffle hands
        hands = st.session_state[AppConfig.HANDS]
        random.shuffle(hands)
