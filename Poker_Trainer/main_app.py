"""
Main Application - Simplified and Modular Poker Range Trainer
"""

import streamlit as st
import time
from datetime import datetime
from typing import Optional

# Core modules
from core.user_manager import UserManager, User
from core.session_manager import SessionManager, TrainingSession
from config.settings import app_config, training_config
from components.range_trainer import RangeTrainer
from components.card_display import CardDisplay
from utils.poker_logic import PokerLogic

class PokerTrainerApp:
    """Main application class"""
    
    def __init__(self):
        self.user_manager = UserManager()
        self.session_manager = SessionManager()
        self.range_trainer = RangeTrainer()
        self.card_display = CardDisplay()
        self.poker_logic = PokerLogic()
        
        # Initialize session state
        self.init_session_state()
    
    def init_session_state(self):
        """Initialize Streamlit session state"""
        if 'current_user' not in st.session_state:
            st.session_state.current_user = None
        
        if 'current_session' not in st.session_state:
            st.session_state.current_session = None
        
        if 'current_scenario' not in st.session_state:
            st.session_state.current_scenario = None
        
        if 'current_streak' not in st.session_state:
            st.session_state.current_streak = 0
        
        if 'response_start_time' not in st.session_state:
            st.session_state.response_start_time = None
    
    def setup_page_config(self):
        """Setup Streamlit page configuration"""
        st.set_page_config(
            page_title=app_config.APP_NAME,
            page_icon=app_config.PAGE_ICON,
            layout="wide",
            initial_sidebar_state="expanded"
        )
        
        # Apply custom CSS
        st.markdown(f"""
        <style>
            .main-header {{
                background: linear-gradient(135deg, {app_config.THEME["primary_color"]} 0%, {app_config.THEME["secondary_color"]} 100%);
                padding: 1rem;
                border-radius: 10px;
                color: white;
                text-align: center;
                margin-bottom: 2rem;
            }}
            
            .card-container {{
                background: {app_config.THEME["background_color"]};
                border-radius: 15px;
                padding: 20px;
                margin: 10px 0;
                border: 2px solid {app_config.THEME["accent_color"]};
            }}
            
            .correct-feedback {{
                background: #4CAF50;
                color: white;
                padding: 10px;
                border-radius: 8px;
                margin: 10px 0;
            }}
            
            .incorrect-feedback {{
                background: #F44336;
                color: white;
                padding: 10px;
                border-radius: 8px;
                margin: 10px 0;
            }}
            
            .user-info {{
                background: {app_config.THEME["accent_color"]};
                color: {app_config.THEME["primary_color"]};
                padding: 10px;
                border-radius: 8px;
                margin: 10px 0;
                font-weight: bold;
            }}
        </style>
        """, unsafe_allow_html=True)
    
    def render_user_login(self):
        """Render user login/registration interface"""
        st.markdown(f'<div class="main-header"><h1>{app_config.PAGE_ICON} {app_config.APP_NAME}</h1><p>Multi-User Poker Range Training</p></div>', unsafe_allow_html=True)
        
        col1, col2, col3 = st.columns([1, 2, 1])
        
        with col2:
            st.subheader("Login or Register")
            
            tab1, tab2 = st.tabs(["Login", "Register"])
            
            with tab1:
                username = st.text_input("Username", key="login_username")
                pin = st.text_input("4-Digit PIN", key="login_pin", type="password", max_chars=4)
                if st.button("Login", key="login_btn"):
                    if username and pin and len(pin) == 4 and pin.isdigit():
                        user = self.user_manager.verify_user_pin(username, pin)
                        if user:
                            st.session_state.current_user = user
                            self.user_manager.update_user_activity(user.id)
                            st.success(f"Welcome back, {user.display_name}!")
                            time.sleep(1)
                            st.rerun()
                        else:
                            st.error("Invalid username or PIN.")
                    else:
                        st.error("Please enter a valid username and 4-digit PIN.")
            
            with tab2:
                reg_username = st.text_input("Username", key="reg_username")
                reg_display_name = st.text_input("Display Name", key="reg_display_name")
                reg_pin = st.text_input("Create 4-Digit PIN", key="reg_pin", type="password", max_chars=4)
                reg_pin_confirm = st.text_input("Confirm PIN", key="reg_pin_confirm", type="password", max_chars=4)
                
                if st.button("Register", key="reg_btn"):
                    if reg_username and reg_display_name and reg_pin and reg_pin_confirm:
                        if len(reg_pin) == 4 and reg_pin.isdigit():
                            if reg_pin == reg_pin_confirm:
                                try:
                                    user = self.user_manager.create_user(reg_username, reg_display_name, reg_pin)
                                    st.session_state.current_user = user
                                    st.success(f"Welcome, {user.display_name}!")
                                    time.sleep(1)
                                    st.rerun()
                                except ValueError as e:
                                    st.error(str(e))
                            else:
                                st.error("PINs do not match.")
                        else:
                            st.error("PIN must be exactly 4 digits.")
                    else:
                        st.error("Please fill in all fields.")
            
            # Show existing users for reference
            st.subheader("Recent Users")
            users = self.user_manager.get_all_users()
            if users:
                st.info("Recent users: " + ", ".join([user.display_name for user in users[:5]]))
            else:
                st.info("No users registered yet.")
    
    def render_session_setup(self):
        """Render session setup interface"""
        user = st.session_state.current_user
        
        with st.sidebar:
            st.markdown(f'<div class="user-info">👤 {user.display_name}</div>', unsafe_allow_html=True)
            
            if st.button("🚪 Logout"):
                st.session_state.current_user = None
                st.session_state.current_session = None
                st.rerun()
            
            st.divider()
            
            # User stats
            stats = self.user_manager.get_user_stats(user.id)
            st.subheader("📊 Your Stats")
            col1, col2 = st.columns(2)
            with col1:
                st.metric("Total Sessions", stats['total_sessions'])
                st.metric("Best Streak", stats['best_streak'])
            with col2:
                st.metric("Total Hands", stats['total_hands'])
                st.metric("Accuracy", f"{stats['overall_accuracy']:.1f}%")
        
        # Main content
        st.markdown(f'<div class="main-header"><h1>{app_config.PAGE_ICON} Training Setup</h1><p>Configure your training session</p></div>', unsafe_allow_html=True)
        
        col1, col2 = st.columns([1, 1])
        
        with col1:
            st.subheader("🎯 Training Configuration")
            
            training_mode = st.selectbox(
                "Training Mode",
                training_config.TRAINING_MODES,
                help="Select the type of training you want to practice"
            )
            
            position = st.selectbox(
                "Position",
                training_config.POSITIONS,
                help="Select your table position (Random for mixed practice)"
            )
            
            stack_depth = st.slider(
                "Stack Depth (BB)",
                min_value=training_config.STACK_DEPTHS["min"],
                max_value=training_config.STACK_DEPTHS["max"],
                value=training_config.STACK_DEPTHS["default"],
                step=training_config.STACK_DEPTHS["step"],
                help="Effective stack depth in big blinds"
            )
        
        with col2:
            st.subheader("📈 Recent Sessions")
            history = self.session_manager.get_session_history(user.id, 5)
            
            if history:
                for session in history:
                    accuracy = (session.correct_answers / max(session.total_hands, 1)) * 100
                    st.markdown(f"""
                    **{session.training_mode}** - {session.position}  
                    Hands: {session.total_hands} | Accuracy: {accuracy:.1f}% | Streak: {session.best_streak}
                    """)
            else:
                st.info("No previous sessions found. Start your first session below!")
        
        # Start session button
        if st.button("🚀 Start Training Session", type="primary", use_container_width=True):
            session = self.session_manager.create_session(
                user.id, training_mode, position, stack_depth
            )
            st.session_state.current_session = session
            st.session_state.current_streak = 0
            st.success("Training session started!")
            time.sleep(1)
            st.rerun()
    
    def render_training_interface(self):
        """Render the main training interface"""
        user = st.session_state.current_user
        session = st.session_state.current_session
        
        # Generate scenario if needed
        if not st.session_state.current_scenario:
            st.session_state.current_scenario = self.range_trainer.generate_scenario(
                session.training_mode, session.position, session.stack_depth
            )
            st.session_state.response_start_time = time.time()
        
        scenario = st.session_state.current_scenario
        
        # Sidebar with session stats
        with st.sidebar:
            st.markdown(f'<div class="user-info">👤 {user.display_name}</div>', unsafe_allow_html=True)
            
            if st.button("🏁 End Session"):
                self.session_manager.end_session(session.id)
                st.session_state.current_session = None
                st.session_state.current_scenario = None
                st.success("Session ended!")
                time.sleep(1)
                st.rerun()
            
            st.divider()
            
            # Live session stats
            active_session = self.session_manager.get_active_session(user.id)
            if active_session:
                st.subheader("📊 Current Session")
                col1, col2 = st.columns(2)
                with col1:
                    st.metric("Hands", active_session.total_hands)
                    st.metric("Streak", st.session_state.current_streak)
                with col2:
                    accuracy = (active_session.correct_answers / max(active_session.total_hands, 1)) * 100
                    st.metric("Accuracy", f"{accuracy:.1f}%")
                    st.metric("Score", active_session.final_score)
                
                # Session duration
                duration = datetime.now() - active_session.start_time
                st.metric("Duration", str(duration).split('.')[0])
        
        # Main training interface
        st.markdown(f'<div class="main-header"><h1>{app_config.PAGE_ICON} Training Session</h1><p>{session.training_mode} - {session.position}</p></div>', unsafe_allow_html=True)
        
        # Display current hand
        col1, col2, col3 = st.columns([1, 2, 1])
        
        with col2:
            st.markdown('<div class="card-container">', unsafe_allow_html=True)
            
            # Hand display
            hand_html = self.card_display.render_hand(scenario['hand'])
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
        
        # Determine available actions based on scenario
        actions = self.get_available_actions(scenario)
        
        # Create action buttons
        button_cols = st.columns(len(actions))
        
        for i, action in enumerate(actions):
            with button_cols[i]:
                if st.button(action, key=f"action_{action}", use_container_width=True):
                    self.handle_action(action, scenario)
    
    def get_available_actions(self, scenario):
        """Get available actions based on scenario type"""
        scenario_type = scenario['scenario_type'].lower()
        
        if 'opening' in scenario_type:
            return ['Fold', 'Raise']
        elif '3bet' in scenario_type or '3-bet' in scenario_type:
            return ['Fold', 'Call', '3-Bet']
        elif '4bet' in scenario_type or '4-bet' in scenario_type:
            return ['Fold', 'Call', '4-Bet']
        else:
            return ['Fold', 'Call', 'Raise']
    
    def handle_action(self, user_action: str, scenario: dict):
        """Handle user action and provide feedback"""
        user = st.session_state.current_user
        session = st.session_state.current_session
        
        # Calculate response time
        response_time = time.time() - st.session_state.response_start_time
        
        # Get correct action
        correct_action = self.poker_logic.get_correct_action(scenario)
        is_correct = user_action.lower() == correct_action.lower()
        
        # Update streak
        if is_correct:
            st.session_state.current_streak += 1
        else:
            st.session_state.current_streak = 0
        
        # Update session stats
        self.session_manager.update_session_stats(
            session.id, is_correct, st.session_state.current_streak
        )
        
        # Save training result
        result = {
            'hand': scenario['hand'],
            'position': scenario['position'],
            'scenario_type': scenario['scenario_type'],
            'user_action': user_action,
            'correct_action': correct_action,
            'is_correct': is_correct,
            'response_time': response_time
        }
        
        self.session_manager.save_training_result(session.id, user.id, result)
        
        # Show feedback
        if is_correct:
            st.markdown(f'<div class="correct-feedback">✅ Correct! {user_action} is the right play.</div>', 
                       unsafe_allow_html=True)
        else:
            st.markdown(f'<div class="incorrect-feedback">❌ Incorrect. The correct action was {correct_action}.</div>', 
                       unsafe_allow_html=True)
        
        # Generate new scenario
        st.session_state.current_scenario = None
        st.session_state.response_start_time = None
        
        # Auto-advance
        time.sleep(2)
        st.rerun()
    
    def run(self):
        """Main application runner"""
        self.setup_page_config()
        
        # Route based on current state
        if not st.session_state.current_user:
            self.render_user_login()
        elif not st.session_state.current_session:
            self.render_session_setup()
        else:
            self.render_training_interface()

# Main execution
if __name__ == "__main__":
    app = PokerTrainerApp()
    app.run()
