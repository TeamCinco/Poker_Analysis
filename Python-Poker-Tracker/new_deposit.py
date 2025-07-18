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
    
    def get_current_bankroll(self):
        """Get the current bankroll from the last session"""
        if not self.sessions:
            return 0
        return self.sessions[-1]['cash_out']
    
    def add_session(self, buy_in: float, cash_out: float, fees: float = 0, notes: str = ""):
        current_bankroll = self.get_current_bankroll()
        
        # If buy_in is 0, this is a continuation session
        if buy_in == 0:
            actual_buy_in = current_bankroll
            new_deposit = 0
            print(f"Continuing with current bankroll: ${current_bankroll:.2f}")
        else:
            actual_buy_in = buy_in
            new_deposit = buy_in
        
        session = {
            "date": datetime.datetime.now().isoformat(),
            "buy_in": actual_buy_in,
            "new_deposit": new_deposit,  # Track actual new money deposited
            "cash_out": cash_out,
            "fees": fees,
            "profit_loss": cash_out - actual_buy_in - fees,
            "notes": notes
        }
        self.sessions.append(session)
        self.save_sessions()
        
        if session['profit_loss'] > 0:
            print(f"Session added: +${session['profit_loss']:.2f}")
        else:
            print(f"Session added: ${session['profit_loss']:.2f}")
    
    def get_stats(self):
        if not self.sessions:
            return {
                "total_sessions": 0, 
                "total_profit_loss": 0, 
                "total_fees": 0, 
                "total_deposits": 0,
                "total_cash_outs": 0,
                "current_bankroll": 0,
                "average_session": 0
            }
        
        # Handle both old and new session formats
        total_deposits = 0
        for session in self.sessions:
            if 'new_deposit' in session:
                total_deposits += session['new_deposit']
            else:
                # For old sessions, treat buy_in as deposit
                total_deposits += session['buy_in']
        
        total_fees = sum(s['fees'] for s in self.sessions)
        current_bankroll = self.get_current_bankroll()
        
        # Total profit/loss = current bankroll - total deposits - total fees
        total_profit_loss = current_bankroll - total_deposits - total_fees
        
        return {
            "total_sessions": len(self.sessions),
            "total_deposits": total_deposits,
            "total_cash_outs": current_bankroll,
            "total_fees": total_fees,
            "total_profit_loss": total_profit_loss,
            "current_bankroll": current_bankroll,
            "average_session": sum(s['profit_loss'] for s in self.sessions) / len(self.sessions)
        }