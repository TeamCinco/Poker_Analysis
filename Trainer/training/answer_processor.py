"""Answer processing logic for training sessions."""

import streamlit as st
from typing import Tuple
from config.settings import AppConfig
from core.session_manager import SessionManager

class AnswerProcessor:
    """Process user answers and update training state."""
    
    @staticmethod
    def process_answer(user_answer: bool):
        """Process the user's answer and update session state."""
        current_hand, position, correct_answer = SessionManager.get_current_hand()
        
        # Create answer result
        answer_result = {
            'hand': current_hand,
            'position': position,
            'user_answer': user_answer,
            'correct_answer': correct_answer,
            'correct': user_answer == correct_answer
        }
        
        # Store the result
        st.session_state[AppConfig.LAST_ANSWER] = answer_result
        
        # Update score if correct
        if user_answer == correct_answer:
            st.session_state[AppConfig.SCORE] += 1
        
        # Update total answered count
        st.session_state[AppConfig.TOTAL_ANSWERED] += 1
        
        # Show result
        st.session_state[AppConfig.SHOW_RESULT] = True
    
    @staticmethod
    def handle_fold_answer():
        """Handle when user clicks FOLD."""
        AnswerProcessor.process_answer(False)
    
    @staticmethod
    def handle_play_answer():
        """Handle when user clicks PLAY."""
        AnswerProcessor.process_answer(True)
    
    @staticmethod
    def get_answer_feedback() -> dict:
        """Get feedback data for the last answer."""
        return SessionManager.get_last_answer()
    
    @staticmethod
    def is_answer_correct(user_answer: bool, correct_answer: bool) -> bool:
        """Check if the user's answer is correct."""
        return user_answer == correct_answer
    
    @staticmethod
    def calculate_accuracy(score: int, total: int) -> float:
        """Calculate accuracy percentage."""
        if total == 0:
            return 0.0
        return (score / total) * 100.0
