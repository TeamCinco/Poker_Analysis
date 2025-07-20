"""Session state management for the Poker Trainer."""

import streamlit as st
import random
from typing import List, Tuple, Optional, Dict
from config.settings import AppConfig
from config.position_options import PositionConfig
from data.position_filters import PositionFilter
from data.excel_data_loader import ExcelDataLoader

class SessionManager:
    """Manage Streamlit session state for the poker trainer."""
    
    # Session state keys for new features
    TABLE_SIZE_KEY = "table_size"
    POSITION_KEY = "position"
    ACTION_KEY = "action"
    TRAINING_MODE_KEY = "training_mode"
    EXCEL_LOADER_KEY = "excel_loader"
    
    @staticmethod
    def initialize_session_state():
        """Initialize all session state variables."""
        # Initialize Excel data loader
        if SessionManager.EXCEL_LOADER_KEY not in st.session_state:
            st.session_state[SessionManager.EXCEL_LOADER_KEY] = ExcelDataLoader()
        
        # Initialize training configuration
        if SessionManager.TABLE_SIZE_KEY not in st.session_state:
            available_table_sizes = st.session_state[SessionManager.EXCEL_LOADER_KEY].get_available_table_sizes()
            st.session_state[SessionManager.TABLE_SIZE_KEY] = available_table_sizes[0] if available_table_sizes else "6-max"
        
        if SessionManager.POSITION_KEY not in st.session_state:
            table_size = st.session_state[SessionManager.TABLE_SIZE_KEY]
            available_positions = PositionConfig.get_positions_for_table_size(table_size)
            st.session_state[SessionManager.POSITION_KEY] = available_positions[0] if available_positions else "UTG"
        
        if SessionManager.ACTION_KEY not in st.session_state:
            st.session_state[SessionManager.ACTION_KEY] = "Opening-Raise"
        
        if SessionManager.TRAINING_MODE_KEY not in st.session_state:
            st.session_state[SessionManager.TRAINING_MODE_KEY] = "single_scenario"
        
        # Legacy support - Default position selection
        if AppConfig.SELECTED_POSITION not in st.session_state:
            st.session_state[AppConfig.SELECTED_POSITION] = "all"
        
        # Initialize hands list based on current configuration
        if AppConfig.HANDS not in st.session_state:
            SessionManager._refresh_hands_list()
        
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
    def _refresh_hands_list():
        """Refresh the hands list based on current training configuration."""
        loader = st.session_state[SessionManager.EXCEL_LOADER_KEY]
        
        # Check if we're using new training mode or legacy mode
        training_mode = st.session_state.get(SessionManager.TRAINING_MODE_KEY, "single_scenario")
        
        if training_mode == "single_scenario":
            # Single position/action scenario
            table_size = st.session_state[SessionManager.TABLE_SIZE_KEY]
            position = st.session_state[SessionManager.POSITION_KEY]
            action = st.session_state[SessionManager.ACTION_KEY]
            
            hands = loader.get_training_hands_for_scenario(table_size, position, action)
            st.session_state[AppConfig.HANDS] = hands
        else:
            # Fall back to legacy mode for other training modes (can be expanded later)
            selected_position = st.session_state[AppConfig.SELECTED_POSITION]
            st.session_state[AppConfig.HANDS] = PositionFilter.get_hands_for_position(selected_position)
        
        # Shuffle the hands
        random.shuffle(st.session_state[AppConfig.HANDS])
    
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
        
        # Refresh and reshuffle hands
        SessionManager._refresh_hands_list()
        random.shuffle(st.session_state[AppConfig.HANDS])
    
    # New methods for enhanced training configuration
    @staticmethod
    def get_current_training_config() -> Dict[str, str]:
        """Get current training configuration."""
        return {
            "table_size": st.session_state.get(SessionManager.TABLE_SIZE_KEY, "6-max"),
            "position": st.session_state.get(SessionManager.POSITION_KEY, "UTG"),
            "action": st.session_state.get(SessionManager.ACTION_KEY, "Opening-Raise"),
            "training_mode": st.session_state.get(SessionManager.TRAINING_MODE_KEY, "single_scenario")
        }
    
    @staticmethod
    def update_table_size(new_table_size: str):
        """Update table size and refresh dependent configurations."""
        if new_table_size != st.session_state.get(SessionManager.TABLE_SIZE_KEY):
            st.session_state[SessionManager.TABLE_SIZE_KEY] = new_table_size
            
            # Update position to first valid position for new table size
            available_positions = PositionConfig.get_positions_for_table_size(new_table_size)
            if available_positions:
                st.session_state[SessionManager.POSITION_KEY] = available_positions[0]
            
            # Refresh hands and reset training state
            SessionManager._refresh_hands_list()
            st.session_state[AppConfig.CURRENT_HAND_INDEX] = 0
            st.session_state[AppConfig.SHOW_RESULT] = False
            st.session_state[AppConfig.LAST_ANSWER] = None
    
    @staticmethod
    def update_position(new_position: str):
        """Update position and refresh hands."""
        if new_position != st.session_state.get(SessionManager.POSITION_KEY):
            st.session_state[SessionManager.POSITION_KEY] = new_position
            
            # Refresh hands and reset training state
            SessionManager._refresh_hands_list()
            st.session_state[AppConfig.CURRENT_HAND_INDEX] = 0
            st.session_state[AppConfig.SHOW_RESULT] = False
            st.session_state[AppConfig.LAST_ANSWER] = None
    
    @staticmethod
    def update_action(new_action: str):
        """Update action and refresh hands."""
        if new_action != st.session_state.get(SessionManager.ACTION_KEY):
            st.session_state[SessionManager.ACTION_KEY] = new_action
            
            # Refresh hands and reset training state
            SessionManager._refresh_hands_list()
            st.session_state[AppConfig.CURRENT_HAND_INDEX] = 0
            st.session_state[AppConfig.SHOW_RESULT] = False
            st.session_state[AppConfig.LAST_ANSWER] = None
    
    @staticmethod
    def update_training_mode(new_mode: str):
        """Update training mode and refresh hands."""
        if new_mode != st.session_state.get(SessionManager.TRAINING_MODE_KEY):
            st.session_state[SessionManager.TRAINING_MODE_KEY] = new_mode
            
            # Refresh hands and reset training state
            SessionManager._refresh_hands_list()
            st.session_state[AppConfig.CURRENT_HAND_INDEX] = 0
            st.session_state[AppConfig.SHOW_RESULT] = False
            st.session_state[AppConfig.LAST_ANSWER] = None
    
    @staticmethod
    def get_current_scenario_display() -> str:
        """Get formatted display string for current training scenario."""
        config = SessionManager.get_current_training_config()
        return PositionConfig.format_scenario_display(
            config["table_size"], 
            config["position"], 
            config["action"]
        )
