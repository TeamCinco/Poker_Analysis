"""
Session Management System for Training Sessions
"""

import sqlite3
import uuid
from datetime import datetime
from typing import Dict, List, Optional, Any
from dataclasses import dataclass
from config.settings import database_config, training_config

@dataclass
class TrainingSession:
    """Training session data class"""
    id: str
    user_id: str
    training_mode: str
    position: str
    stack_depth: int
    start_time: datetime
    end_time: Optional[datetime] = None
    total_hands: int = 0
    correct_answers: int = 0
    current_streak: int = 0
    best_streak: int = 0
    final_score: int = 0
    is_active: bool = True

class SessionManager:
    """Manages training sessions for users"""
    
    def __init__(self):
        self.db_path = database_config.DB_PATH
    
    def create_session(self, user_id: str, training_mode: str, position: str, stack_depth: int) -> TrainingSession:
        """Create a new training session"""
        session_id = str(uuid.uuid4())
        
        conn = sqlite3.connect(self.db_path)
        cursor = conn.cursor()
        
        # End any existing active sessions for this user
        cursor.execute('''
            UPDATE training_sessions 
            SET is_active = 0, end_time = CURRENT_TIMESTAMP
            WHERE user_id = ? AND is_active = 1
        ''', (user_id,))
        
        # Create new session
        cursor.execute('''
            INSERT INTO training_sessions 
            (id, user_id, training_mode, position, stack_depth, start_time)
            VALUES (?, ?, ?, ?, ?, ?)
        ''', (session_id, user_id, training_mode, position, stack_depth, datetime.now()))
        
        conn.commit()
        conn.close()
        
        return TrainingSession(
            id=session_id,
            user_id=user_id,
            training_mode=training_mode,
            position=position,
            stack_depth=stack_depth,
            start_time=datetime.now()
        )
    
    def get_active_session(self, user_id: str) -> Optional[TrainingSession]:
        """Get the active session for a user"""
        conn = sqlite3.connect(self.db_path)
        cursor = conn.cursor()
        
        cursor.execute('''
            SELECT id, user_id, training_mode, position, stack_depth, start_time,
                   end_time, total_hands, correct_answers, current_streak, 
                   best_streak, final_score, is_active
            FROM training_sessions 
            WHERE user_id = ? AND is_active = 1
        ''', (user_id,))
        
        result = cursor.fetchone()
        conn.close()
        
        if result:
            # Convert string timestamps back to datetime objects
            session_data = list(result)
            # Try different datetime formats
            try:
                session_data[5] = datetime.strptime(session_data[5], '%Y-%m-%d %H:%M:%S')
            except ValueError:
                try:
                    session_data[5] = datetime.strptime(session_data[5], '%Y-%m-%d %H:%M:%S.%f')
                except ValueError:
                    session_data[5] = datetime.fromisoformat(session_data[5].replace(' ', 'T'))
            
            if session_data[6]:  # end_time
                try:
                    session_data[6] = datetime.strptime(session_data[6], '%Y-%m-%d %H:%M:%S')
                except ValueError:
                    try:
                        session_data[6] = datetime.strptime(session_data[6], '%Y-%m-%d %H:%M:%S.%f')
                    except ValueError:
                        session_data[6] = datetime.fromisoformat(session_data[6].replace(' ', 'T'))
            
            return TrainingSession(*session_data)
        return None
    
    def update_session_stats(self, session_id: str, is_correct: bool, current_streak: int):
        """Update session statistics"""
        conn = sqlite3.connect(self.db_path)
        cursor = conn.cursor()
        
        # Calculate score based on correctness and streak
        score_increase = training_config.SCORING["correct_answer"] if is_correct else 0
        streak_bonus = min(current_streak * training_config.SCORING["streak_bonus"], 
                          training_config.SCORING["max_streak_bonus"])
        
        cursor.execute('''
            UPDATE training_sessions 
            SET total_hands = total_hands + 1,
                correct_answers = correct_answers + ?,
                current_streak = ?,
                best_streak = MAX(best_streak, ?),
                final_score = final_score + ? + ?
            WHERE id = ?
        ''', (1 if is_correct else 0, current_streak, current_streak, 
              score_increase, streak_bonus, session_id))
        
        conn.commit()
        conn.close()
    
    def end_session(self, session_id: str):
        """End a training session"""
        conn = sqlite3.connect(self.db_path)
        cursor = conn.cursor()
        
        cursor.execute('''
            UPDATE training_sessions 
            SET is_active = 0, end_time = CURRENT_TIMESTAMP
            WHERE id = ?
        ''', (session_id,))
        
        # Update user totals
        cursor.execute('''
            UPDATE users 
            SET total_sessions = total_sessions + 1,
                total_hands = total_hands + (
                    SELECT total_hands FROM training_sessions WHERE id = ?
                ),
                total_correct = total_correct + (
                    SELECT correct_answers FROM training_sessions WHERE id = ?
                ),
                best_streak = MAX(best_streak, (
                    SELECT best_streak FROM training_sessions WHERE id = ?
                ))
            WHERE id = (
                SELECT user_id FROM training_sessions WHERE id = ?
            )
        ''', (session_id, session_id, session_id, session_id))
        
        conn.commit()
        conn.close()
    
    def save_training_result(self, session_id: str, user_id: str, result: Dict[str, Any]):
        """Save a training result"""
        conn = sqlite3.connect(self.db_path)
        cursor = conn.cursor()
        
        result_id = str(uuid.uuid4())
        
        cursor.execute('''
            INSERT INTO training_results 
            (id, user_id, session_id, hand, position, scenario_type, 
             user_action, correct_action, is_correct, response_time)
            VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
        ''', (
            result_id, user_id, session_id,
            result['hand'], result['position'], result['scenario_type'],
            result['user_action'], result['correct_action'], result['is_correct'],
            result.get('response_time', 0)
        ))
        
        conn.commit()
        conn.close()
    
    def get_session_history(self, user_id: str, limit: int = 10) -> List[TrainingSession]:
        """Get recent session history for a user"""
        conn = sqlite3.connect(self.db_path)
        cursor = conn.cursor()
        
        cursor.execute('''
            SELECT id, user_id, training_mode, position, stack_depth, start_time,
                   end_time, total_hands, correct_answers, current_streak, 
                   best_streak, final_score, is_active
            FROM training_sessions 
            WHERE user_id = ? AND is_active = 0
            ORDER BY start_time DESC
            LIMIT ?
        ''', (user_id, limit))
        
        results = cursor.fetchall()
        conn.close()
        
        return [TrainingSession(*result) for result in results]
    
    def get_session_stats(self, session_id: str) -> Dict:
        """Get detailed session statistics"""
        conn = sqlite3.connect(self.db_path)
        cursor = conn.cursor()
        
        # Basic session stats
        cursor.execute('''
            SELECT total_hands, correct_answers, best_streak, final_score,
                   training_mode, position, stack_depth, start_time
            FROM training_sessions WHERE id = ?
        ''', (session_id,))
        
        session_data = cursor.fetchone()
        
        # Action breakdown
        cursor.execute('''
            SELECT user_action, COUNT(*) as count,
                   SUM(CASE WHEN is_correct = 1 THEN 1 ELSE 0 END) as correct_count
            FROM training_results 
            WHERE session_id = ?
            GROUP BY user_action
        ''', (session_id,))
        
        action_stats = cursor.fetchall()
        
        conn.close()
        
        if not session_data:
            return {}
        
        return {
            'total_hands': session_data[0],
            'correct_answers': session_data[1],
            'accuracy': (session_data[1] / max(session_data[0], 1)) * 100,
            'best_streak': session_data[2],
            'final_score': session_data[3],
            'training_mode': session_data[4],
            'position': session_data[5],
            'stack_depth': session_data[6],
            'start_time': session_data[7],
            'action_breakdown': {
                action: {'total': count, 'correct': correct, 'accuracy': (correct/count)*100}
                for action, count, correct in action_stats
            }
        }
