"""Reusable UI components for the Poker Trainer."""

import streamlit as st
from config.position_options import PositionConfig
from core.session_manager import SessionManager
from typing import Tuple, Dict

class UIComponents:
    """Reusable UI components."""
    
    @staticmethod
    def display_title():
        """Display the main application title."""
        st.title("♠️ Poker Hand Trainer ♠️")
    
    @staticmethod
    def display_enhanced_training_selector() -> Dict[str, str]:
        """Display enhanced training configuration selector."""
        st.subheader("🎯 Training Configuration")
        
        config = SessionManager.get_current_training_config()
        loader = st.session_state[SessionManager.EXCEL_LOADER_KEY]
        
        col1, col2, col3 = st.columns(3)
        
        with col1:
            # Table Size Selector
            available_table_sizes = loader.get_available_table_sizes()
            if available_table_sizes:
                current_table_size = config["table_size"]
                table_size_index = available_table_sizes.index(current_table_size) if current_table_size in available_table_sizes else 0
                
                selected_table_size = st.selectbox(
                    "Table Size:",
                    options=available_table_sizes,
                    index=table_size_index,
                    key="table_size_selector"
                )
                
                if selected_table_size != current_table_size:
                    SessionManager.update_table_size(selected_table_size)
                    st.rerun()
        
        with col2:
            # Position Selector
            available_positions = PositionConfig.get_positions_for_table_size(config["table_size"])
            if available_positions:
                current_position = config["position"]
                position_index = available_positions.index(current_position) if current_position in available_positions else 0
                
                selected_position = st.selectbox(
                    "Position:",
                    options=available_positions,
                    format_func=lambda x: PositionConfig.get_position_full_name(x),
                    index=position_index,
                    key="position_selector"
                )
                
                if selected_position != current_position:
                    SessionManager.update_position(selected_position)
                    st.rerun()
        
        with col3:
            # Action Selector
            available_actions = PositionConfig.get_available_actions()
            current_action = config["action"]
            action_index = available_actions.index(current_action) if current_action in available_actions else 0
            
            selected_action = st.selectbox(
                "Action:",
                options=available_actions,
                index=action_index,
                key="action_selector"
            )
            
            if selected_action != current_action:
                SessionManager.update_action(selected_action)
                st.rerun()
        
        # Display current scenario
        scenario_display = SessionManager.get_current_scenario_display()
        st.info(f"🎲 Current Scenario: **{scenario_display}**")
        
        return SessionManager.get_current_training_config()
    
    @staticmethod
    def display_position_selector(current_position: str) -> str:
        """Display legacy position selection interface (for backward compatibility)."""
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
