import streamlit as st
import pandas as pd
import numpy as np
import json
import sqlite3
from datetime import datetime, timedelta
import plotly.graph_objects as go
import plotly.express as px
from utils.poker_logic import PokerLogic
from utils.database import Database
from components.range_trainer import RangeTrainer
from components.card_display import CardDisplay

# Page config
st.set_page_config(
    page_title="Poker Range Trainer",
    page_icon="🃏",
    layout="wide",
    initial_sidebar_state="expanded"
)

# Custom CSS for mobile responsiveness and poker theme
st.markdown("""
<style>
    .main-header {
        background: linear-gradient(135deg, #0F4C3A 0%, #2E8B57 100%);
        padding: 1rem;
        border-radius: 10px;
        color: white;
        text-align: center;
        margin-bottom: 2rem;
    }
    
    .card-container {
        background: #1a1a1a;
        border-radius: 15px;
        padding: 20px;
        margin: 10px 0;
        border: 2px solid #FFD700;
    }
    
    .action-button {
        background: #FFD700;
        color: #0F4C3A;
        border: none;
        padding: 10px 20px;
        border-radius: 8px;
        font-weight: bold;
        margin: 5px;
        cursor: pointer;
    }
    
    .correct-feedback {
        background: #4CAF50;
        color: white;
        padding: 10px;
        border-radius: 8px;
        margin: 10px 0;
    }
    
    .incorrect-feedback {
        background: #F44336;
        color: white;
        padding: 10px;
        border-radius: 8px;
        margin: 10px 0;
    }
    
    .streak-counter {
        background: #FFD700;
        color: #0F4C3A;
        padding: 15px;
        border-radius: 50%;
        text-align: center;
        font-weight: bold;
        font-size: 1.2em;
    }
    
    /* Mobile responsiveness */
    @media (max-width: 768px) {
        .main-header {
            padding: 0.5rem;
            font-size: 1.2em;
        }
        
        .card-container {
            padding: 10px;
            margin: 5px 0;
        }
        
        .action-button {
            padding: 8px 16px;
            font-size: 0.9em;
        }
    }
</style>
""", unsafe_allow_html=True)

# Initialize session state
if 'db' not in st.session_state:
    st.session_state.db = Database()
    
if 'poker_logic' not in st.session_state:
    st.session_state.poker_logic = PokerLogic()
    
if 'current_session' not in st.session_state:
    st.session_state.current_session = {
        'score': 0,
        'streak': 0,
        'total_hands': 0,
        'correct_answers': 0,
        'start_time': datetime.now()
    }

# Main header
st.markdown('<div class="main-header"><h1>🃏 Poker Range Trainer</h1><p>Master your preflop ranges, 3-bets, and 4-bets</p></div>', unsafe_allow_html=True)

# Sidebar for navigation and stats
with st.sidebar:
    st.header("📊 Session Stats")
    
    col1, col2 = st.columns(2)
    with col1:
        st.metric("Streak", st.session_state.current_session['streak'])
    with col2:
        accuracy = (st.session_state.current_session['correct_answers'] / 
                   max(st.session_state.current_session['total_hands'], 1)) * 100
        st.metric("Accuracy", f"{accuracy:.1f}%")
    
    st.metric("Total Hands", st.session_state.current_session['total_hands'])
    
    # Session timer
    session_duration = datetime.now() - st.session_state.current_session['start_time']
    st.metric("Session Time", str(session_duration).split('.')[0])
    
    st.divider()
    
    # Training mode selection
    st.header("🎯 Training Mode")
    training_mode = st.selectbox(
        "Select Training Type",
        ["Opening Ranges", "3-Bet Ranges", "4-Bet Ranges", "Calling Ranges", "Mixed Training"]
    )
    
    position = st.selectbox(
        "Position",
        ["UTG", "MP", "CO", "BTN", "SB", "BB", "Random"]
    )
    
    stack_depth = st.slider("Stack Depth (BB)", 20, 200, 100, 10)
    
    # Start/Reset session
    if st.button("🔄 Reset Session"):
        st.session_state.current_session = {
            'score': 0,
            'streak': 0,
            'total_hands': 0,
            'correct_answers': 0,
            'start_time': datetime.now()
        }
        st.rerun()

# Main training interface
def main_training_interface():
    trainer = RangeTrainer()
    
    # Generate current scenario
    if 'current_scenario' not in st.session_state:
        st.session_state.current_scenario = trainer.generate_scenario(
            training_mode, position, stack_depth
        )
    
    scenario = st.session_state.current_scenario
    
    # Display current hand
    col1, col2, col3 = st.columns([1, 2, 1])
    
    with col2:
        st.markdown('<div class="card-container">', unsafe_allow_html=True)
        
        # Hand display
        card_display = CardDisplay()
        hand_html = card_display.render_hand(scenario['hand'])
        st.markdown(hand_html, unsafe_allow_html=True)
        
        # Scenario info
        st.markdown(f"""
        **Position:** {scenario['position']}  
        **Stack Depth:** {scenario['stack_depth']}BB  
        **Scenario:** {scenario['scenario_type']}  
        **Opponent Action:** {scenario.get('opponent_action', 'N/A')}
        """)
        
        st.markdown('</div>', unsafe_allow_html=True)
    
    # Action buttons
    st.markdown("### Choose your action:")
    
    actions = ['Fold', 'Call', 'Raise', '3-Bet', '4-Bet']
    if scenario['scenario_type'] == 'opening':
        actions = ['Fold', 'Raise']
    elif scenario['scenario_type'] == '3bet':
        actions = ['Fold', 'Call', '3-Bet']
    elif scenario['scenario_type'] == '4bet':
        actions = ['Fold', 'Call', '4-Bet']
    
    # Create action buttons in columns
    button_cols = st.columns(len(actions))
    
    for i, action in enumerate(actions):
        with button_cols[i]:
            if st.button(action, key=f"action_{action}", use_container_width=True):
                handle_action(action, scenario)

def handle_action(user_action, scenario):
    """Handle user action and provide feedback"""
    correct_action = st.session_state.poker_logic.get_correct_action(scenario)
    is_correct = user_action.lower() == correct_action.lower()
    
    # Update session stats
    st.session_state.current_session['total_hands'] += 1
    
    if is_correct:
        st.session_state.current_session['correct_answers'] += 1
        st.session_state.current_session['streak'] += 1
        st.session_state.current_session['score'] += 10
        
        st.markdown(f'<div class="correct-feedback">✅ Correct! {user_action} is the right play.</div>', 
                   unsafe_allow_html=True)
    else:
        st.session_state.current_session['streak'] = 0
        
        st.markdown(f'<div class="incorrect-feedback">❌ Incorrect. The correct action was {correct_action}.</div>', 
                   unsafe_allow_html=True)
    
    # Save to database
    st.session_state.db.save_training_result({
        'hand': scenario['hand'],
        'position': scenario['position'],
        'scenario_type': scenario['scenario_type'],
        'user_action': user_action,
        'correct_action': correct_action,
        'is_correct': is_correct,
        'timestamp': datetime.now()
    })
    
    # Generate new scenario
    trainer = RangeTrainer()
    st.session_state.current_scenario = trainer.generate_scenario(
        training_mode, position, stack_depth
    )
    
    # Auto-advance after 2 seconds
    st.success("Next hand loading...")
    st.rerun()

# Run main interface
main_training_interface()

# Quick stats at bottom
st.divider()
col1, col2, col3, col4 = st.columns(4)

with col1:
    st.metric("Session Score", st.session_state.current_session['score'])
with col2:
    st.metric("Best Streak", st.session_state.db.get_best_streak())
with col3:
    st.metric("Total Sessions", st.session_state.db.get_total_sessions())
with col4:
    st.metric("Overall Accuracy", f"{st.session_state.db.get_overall_accuracy():.1f}%")
