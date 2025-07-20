"""Reusable UI components for the Poker Trainer."""

import streamlit as st
from config.position_options import PositionConfig

class UIComponents:
    """Reusable UI components."""
    
    @staticmethod
    def display_title():
        """Display the main application title."""
        st.title("♠️ Poker Hand Trainer ♠️")
    
    @staticmethod
    def display_position_selector(current_position: str) -> str:
        """Display position selection interface."""
        st.subheader("🎯 Select Training Position:")
        
        selected_position = st.selectbox(
            "Choose which position to train:",
            options=list(PositionConfig.POSITION_OPTIONS.keys()),
            format_func=lambda x: PositionConfig.get_position_display_name(x),
            index=list(PositionConfig.POSITION_OPTIONS.keys()).index(current_position)
        )
        
        return selected_position
    
    @staticmethod
    def display_accuracy_stats(score: int, total_answered: int):
        """Display accuracy statistics."""
        if total_answered > 0:
            accuracy = (score / total_answered) * 100
            st.metric("Accuracy", f"{accuracy:.1f}%", 
                     f"{score}/{total_answered}")
    
    @staticmethod
    def display_training_question():
        """Display the main training question."""
        st.subheader("What should you do with this hand?")
    
    @staticmethod
    def display_action_buttons() -> tuple:
        """Display FOLD and PLAY buttons."""
        col1, col2, col3 = st.columns([1, 1, 1])
        
        fold_clicked = False
        play_clicked = False
        
        with col1:
            fold_clicked = st.button("🔴 FOLD", key="fold", use_container_width=True, 
                                   help="Click if you should fold this hand")
        
        with col3:
            play_clicked = st.button("🟢 PLAY", key="play", use_container_width=True,
                                   help="Click if you should play/raise this hand")
        
        return fold_clicked, play_clicked
    
    @staticmethod
    def display_result_feedback(result: dict):
        """Display feedback after user answers."""
        if result['correct']:
            st.success(f"✅ Correct! {result['hand']} should be {'PLAYED' if result['correct_answer'] else 'FOLDED'}")
        else:
            st.error(f"❌ Wrong! {result['hand']} should be {'PLAYED' if result['correct_answer'] else 'FOLDED'}")
        
        # Show position info if it's a playable hand
        if result['position'] != "fold":
            formatted_position = PositionConfig.get_formatted_position_name(result['position'])
            st.info(f"📍 Position: {formatted_position}")
    
    @staticmethod
    def display_next_hand_button() -> bool:
        """Display next hand button and return if clicked."""
        return st.button("Next Hand ➡️", key="next", use_container_width=True)
    
    @staticmethod
    def display_reset_button() -> bool:
        """Display reset training button and return if clicked."""
        st.divider()
        return st.button("🔄 Reset Training", use_container_width=True)
