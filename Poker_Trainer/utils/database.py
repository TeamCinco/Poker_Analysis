# Database utilities
# Database connection and operations
import sqlite3
import pandas as pd
from datetime import datetime
from typing import Dict, List, Any

class Database:
    def __init__(self, db_path: str = "data/user_data.db"):
        self.db_path = db_path
        self.init_database()
    
    def init_database(self):
        """Initialize database tables"""
        conn = sqlite3.connect(self.db_path)
        cursor = conn.cursor()
        
        # Training results table
        cursor.execute('''
            CREATE TABLE IF NOT EXISTS training_results (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                hand TEXT NOT NULL,
                position TEXT NOT NULL,
                scenario_type TEXT NOT NULL,
                user_action TEXT NOT NULL,
                correct_action TEXT NOT NULL,
                is_correct BOOLEAN NOT NULL,
                timestamp DATETIME DEFAULT CURRENT_TIMESTAMP
            )
        ''')
        
        # User sessions table
        cursor.execute('''
            CREATE TABLE IF NOT EXISTS user_sessions (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                start_time DATETIME NOT NULL,
                end_time DATETIME,
                total_hands INTEGER DEFAULT 0,
                correct_answers INTEGER DEFAULT 0,
                best_streak INTEGER DEFAULT 0,
                final_score INTEGER DEFAULT 0
            )
        ''')
        
        conn.commit()
        conn.close()
    
    def save_training_result(self, result: Dict[str, Any]):
        """Save a training result to database"""
        conn = sqlite3.connect(self.db_path)
        cursor = conn.cursor()
        
        cursor.execute('''
            INSERT INTO training_results 
            (hand, position, scenario_type, user_action, correct_action, is_correct, timestamp)
            VALUES (?, ?, ?, ?, ?, ?, ?)
        ''', (
            result['hand'],
            result['position'],
            result['scenario_type'],
            result['user_action'],
            result['correct_action'],
            result['is_correct'],
            result['timestamp']
        ))
        
        conn.commit()
        conn.close()
    
    def get_best_streak(self) -> int:
        """Get the best streak from database"""
        conn = sqlite3.connect(self.db_path)
        cursor = conn.cursor()
        
        cursor.execute('SELECT MAX(best_streak) FROM user_sessions')
        result = cursor.fetchone()
        
        conn.close()
        return result[0] if result[0] else 0
    
    def get_total_sessions(self) -> int:
        """Get total number of sessions"""
        conn = sqlite3.connect(self.db_path)
        cursor = conn.cursor()
        
        cursor.execute('SELECT COUNT(*) FROM user_sessions')
        result = cursor.fetchone()
        
        conn.close()
        return result[0] if result[0] else 0
    
    def get_overall_accuracy(self) -> float:
        """Get overall accuracy percentage"""
        conn = sqlite3.connect(self.db_path)
        cursor = conn.cursor()
        
        cursor.execute('''
            SELECT 
                COUNT(*) as total,
                SUM(CASE WHEN is_correct = 1 THEN 1 ELSE 0 END) as correct
            FROM training_results
        ''')
        result = cursor.fetchone()
        
        conn.close()
        
        if result[0] > 0:
            return (result[1] / result[0]) * 100
        return 0.0
    
    def get_progress_data(self) -> pd.DataFrame:
        """Get progress data for analytics"""
        conn = sqlite3.connect(self.db_path)
        df = pd.read_sql_query('''
            SELECT 
                DATE(timestamp) as date,
                COUNT(*) as total_hands,
                SUM(CASE WHEN is_correct = 1 THEN 1 ELSE 0 END) as correct_hands,
                AVG(CASE WHEN is_correct = 1 THEN 1.0 ELSE 0.0 END) * 100 as accuracy
            FROM training_results
            GROUP BY DATE(timestamp)
            ORDER BY date
        ''', conn)
        
        conn.close()
        return df