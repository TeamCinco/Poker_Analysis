"""
User Management System for Multi-User Poker Training
"""

import sqlite3
import hashlib
import uuid
from datetime import datetime
from typing import Dict, List, Optional, Tuple
from dataclasses import dataclass
from config.settings import database_config

@dataclass
class User:
    """User data class"""
    id: str
    username: str
    display_name: str
    created_at: datetime
    last_active: datetime
    total_sessions: int = 0
    total_hands: int = 0
    total_correct: int = 0
    best_streak: int = 0

class UserManager:
    """Manages user accounts and sessions"""
    
    def __init__(self):
        self.db_path = database_config.DB_PATH
        self.init_user_tables()
    
    def init_user_tables(self):
        """Initialize user-related database tables"""
        conn = sqlite3.connect(self.db_path)
        cursor = conn.cursor()
        
        # Users table
        cursor.execute('''
            CREATE TABLE IF NOT EXISTS users (
                id TEXT PRIMARY KEY,
                username TEXT UNIQUE NOT NULL,
                display_name TEXT NOT NULL,
                pin_hash TEXT NOT NULL,
                created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
                last_active DATETIME DEFAULT CURRENT_TIMESTAMP,
                total_sessions INTEGER DEFAULT 0,
                total_hands INTEGER DEFAULT 0,
                total_correct INTEGER DEFAULT 0,
                best_streak INTEGER DEFAULT 0
            )
        ''')
        
        # Training sessions table (per user)
        cursor.execute('''
            CREATE TABLE IF NOT EXISTS training_sessions (
                id TEXT PRIMARY KEY,
                user_id TEXT NOT NULL,
                training_mode TEXT NOT NULL,
                position TEXT NOT NULL,
                stack_depth INTEGER NOT NULL,
                start_time DATETIME DEFAULT CURRENT_TIMESTAMP,
                end_time DATETIME,
                total_hands INTEGER DEFAULT 0,
                correct_answers INTEGER DEFAULT 0,
                current_streak INTEGER DEFAULT 0,
                best_streak INTEGER DEFAULT 0,
                final_score INTEGER DEFAULT 0,
                is_active BOOLEAN DEFAULT 1,
                FOREIGN KEY (user_id) REFERENCES users (id)
            )
        ''')
        
        # Training results table (per user)
        cursor.execute('''
            CREATE TABLE IF NOT EXISTS training_results (
                id TEXT PRIMARY KEY,
                user_id TEXT NOT NULL,
                session_id TEXT NOT NULL,
                hand TEXT NOT NULL,
                position TEXT NOT NULL,
                scenario_type TEXT NOT NULL,
                user_action TEXT NOT NULL,
                correct_action TEXT NOT NULL,
                is_correct BOOLEAN NOT NULL,
                response_time FLOAT,
                timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
                FOREIGN KEY (user_id) REFERENCES users (id),
                FOREIGN KEY (session_id) REFERENCES training_sessions (id)
            )
        ''')
        
        conn.commit()
        conn.close()
    
    def create_user(self, username: str, display_name: str, pin: str) -> User:
        """Create a new user with PIN"""
        user_id = str(uuid.uuid4())
        
        # Hash the PIN for security
        pin_hash = hashlib.sha256(pin.encode()).hexdigest()
        
        conn = sqlite3.connect(self.db_path)
        cursor = conn.cursor()
        
        try:
            cursor.execute('''
                INSERT INTO users (id, username, display_name, pin_hash)
                VALUES (?, ?, ?, ?)
            ''', (user_id, username, display_name, pin_hash))
            
            conn.commit()
            
            return User(
                id=user_id,
                username=username,
                display_name=display_name,
                created_at=datetime.now(),
                last_active=datetime.now()
            )
            
        except sqlite3.IntegrityError:
            raise ValueError(f"Username '{username}' already exists")
        finally:
            conn.close()
    
    def verify_user_pin(self, username: str, pin: str) -> Optional[User]:
        """Verify user PIN and return user if valid"""
        pin_hash = hashlib.sha256(pin.encode()).hexdigest()
        
        conn = sqlite3.connect(self.db_path)
        cursor = conn.cursor()
        
        cursor.execute('''
            SELECT id, username, display_name, created_at, last_active,
                   total_sessions, total_hands, total_correct, best_streak
            FROM users WHERE username = ? AND pin_hash = ?
        ''', (username, pin_hash))
        
        result = cursor.fetchone()
        conn.close()
        
        if result:
            return User(*result)
        return None
    
    def get_user_by_username(self, username: str) -> Optional[User]:
        """Get user by username"""
        conn = sqlite3.connect(self.db_path)
        cursor = conn.cursor()
        
        cursor.execute('''
            SELECT id, username, display_name, created_at, last_active,
                   total_sessions, total_hands, total_correct, best_streak
            FROM users WHERE username = ?
        ''', (username,))
        
        result = cursor.fetchone()
        conn.close()
        
        if result:
            return User(*result)
        return None
    
    def get_user_by_id(self, user_id: str) -> Optional[User]:
        """Get user by ID"""
        conn = sqlite3.connect(self.db_path)
        cursor = conn.cursor()
        
        cursor.execute('''
            SELECT id, username, display_name, created_at, last_active,
                   total_sessions, total_hands, total_correct, best_streak
            FROM users WHERE id = ?
        ''', (user_id,))
        
        result = cursor.fetchone()
        conn.close()
        
        if result:
            return User(*result)
        return None
    
    def update_user_activity(self, user_id: str):
        """Update user's last active timestamp"""
        conn = sqlite3.connect(self.db_path)
        cursor = conn.cursor()
        
        cursor.execute('''
            UPDATE users 
            SET last_active = CURRENT_TIMESTAMP
            WHERE id = ?
        ''', (user_id,))
        
        conn.commit()
        conn.close()
    
    def get_all_users(self) -> List[User]:
        """Get all users"""
        conn = sqlite3.connect(self.db_path)
        cursor = conn.cursor()
        
        cursor.execute('''
            SELECT id, username, display_name, created_at, last_active,
                   total_sessions, total_hands, total_correct, best_streak
            FROM users
            ORDER BY last_active DESC
        ''')
        
        results = cursor.fetchall()
        conn.close()
        
        return [User(*result) for result in results]
    
    def get_user_stats(self, user_id: str) -> Dict:
        """Get comprehensive user statistics"""
        conn = sqlite3.connect(self.db_path)
        cursor = conn.cursor()
        
        # Basic stats
        cursor.execute('''
            SELECT total_sessions, total_hands, total_correct, best_streak
            FROM users WHERE id = ?
        ''', (user_id,))
        
        user_stats = cursor.fetchone()
        
        # Recent session stats
        cursor.execute('''
            SELECT AVG(correct_answers * 100.0 / total_hands) as avg_accuracy,
                   MAX(best_streak) as session_best_streak,
                   COUNT(*) as recent_sessions
            FROM training_sessions 
            WHERE user_id = ? AND total_hands > 0
            AND start_time > datetime('now', '-30 days')
        ''', (user_id,))
        
        recent_stats = cursor.fetchone()
        
        conn.close()
        
        return {
            'total_sessions': user_stats[0] or 0,
            'total_hands': user_stats[1] or 0,
            'total_correct': user_stats[2] or 0,
            'best_streak': user_stats[3] or 0,
            'overall_accuracy': (user_stats[2] / max(user_stats[1], 1)) * 100,
            'recent_avg_accuracy': recent_stats[0] or 0,
            'recent_sessions': recent_stats[2] or 0
        }
