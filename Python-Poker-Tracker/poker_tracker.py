import json
import datetime
from typing import List, Dict

class PokerTracker:
    def __init__(self, filename="poker_sessions.json"):
        self.filename = filename
        self.sessions = self.load_sessions()
    
    def load_sessions(self) -> List[Dict]:
        try:
            with open(self.filename, 'r') as f:
                return json.load(f)
        except FileNotFoundError:
            return []
    
    def save_sessions(self):
        with open(self.filename, 'w') as f:
            json.dump(self.sessions, f, indent=2)
    
    def add_session(self, buy_in: float, cash_out: float, fees: float = 0, notes: str = ""):
        session = {
            "date": datetime.datetime.now().isoformat(),
            "buy_in": buy_in,
            "cash_out": cash_out,
            "fees": fees,
            "profit_loss": cash_out - buy_in - fees,
            "notes": notes
        }
        self.sessions.append(session)
        self.save_sessions()
        print(f"Session added: ${session['profit_loss']:.2f}")
    
    def get_stats(self):
        if not self.sessions:
            return {
                "total_sessions": 0, 
                "total_profit_loss": 0, 
                "total_fees": 0, 
                "total_buy_ins": 0,
                "total_cash_outs": 0,
                "average_session": 0
            }
        
        total_buy_ins = sum(s['buy_in'] for s in self.sessions)
        total_cash_outs = sum(s['cash_out'] for s in self.sessions)
        total_fees = sum(s['fees'] for s in self.sessions)
        total_profit_loss = sum(s['profit_loss'] for s in self.sessions)
        
        return {
            "total_sessions": len(self.sessions),
            "total_buy_ins": total_buy_ins,
            "total_cash_outs": total_cash_outs,
            "total_fees": total_fees,
            "total_profit_loss": total_profit_loss,
            "average_session": total_profit_loss / len(self.sessions)
        }