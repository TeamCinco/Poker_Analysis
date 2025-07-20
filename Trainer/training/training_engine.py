"""Main training engine that coordinates the poker training workflow."""

import streamlit as st
from core.session_manager import SessionManager
from training.answer_processor import AnswerProcessor
from ui.components import UIComponents
from ui.card_display import CardDisplay

class TrainingEngine:
    """Main training engine that orchestrates the training workflow."""
    
    def __init__(self):
        """Initialize the training engine."""
        SessionManager.initialize_session_state()
    
    def run_training_loop(self):
        """Run the main training loop."""
        # Handle position selection
        current_position = SessionManager.get_selected_position()
        selected_position = UIComponents.display_position_selector(current_position)
        
        # Update position if changed
        SessionManager.update_position(selected_position)
        
        # Display accuracy stats
        score, total_answered = SessionManager.get_score_data()
        UIComponents.display_accuracy_stats(score, total_answered)
        
        st.divider()
        
        # Main training workflow
        if SessionManager.is_showing_result():
            self._handle_result_display()
        else:
            self._handle_question_display()
        
        # Reset button
        if UIComponents.display_reset_button():
            SessionManager.reset_training()
            st.rerun()
    
    def _handle_question_display(self):
        """Handle displaying the training question."""
        current_hand, position, correct_answer = SessionManager.get_current_hand()
        
        # Display question
        UIComponents.display_training_question()
        
        # Display the visual cards
        card_html = CardDisplay.display_cards(current_hand)
        st.markdown(card_html, unsafe_allow_html=True)
        
        # Handle user interaction
        fold_clicked, play_clicked = UIComponents.display_action_buttons()
        
        if fold_clicked:
            AnswerProcessor.handle_fold_answer()
            st.rerun()
        elif play_clicked:
            AnswerProcessor.handle_play_answer()
            st.rerun()
    
    def _handle_result_display(self):
        """Handle displaying the result feedback."""
        result = SessionManager.get_last_answer()
        
        if result:
            # Display the visual cards
            card_html = CardDisplay.display_cards(result['hand'])
            st.markdown(card_html, unsafe_allow_html=True)
            
            # Display feedback
            UIComponents.display_result_feedback(result)
            
            # Next hand button
            if UIComponents.display_next_hand_button():
                SessionManager.advance_to_next_hand()
                st.rerun()
    
    def start_training(self):
        """Start the training session."""
        UIComponents.display_title()
        self.run_training_loop()
