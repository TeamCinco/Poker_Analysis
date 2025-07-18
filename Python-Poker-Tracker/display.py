import datetime

class Display:
    @staticmethod
    def show_stats(stats):
        print("\n" + "="*40)
        print("POKER TRACKER STATS")
        print("="*40)
        print(f"Total Sessions: {stats['total_sessions']}")
        print(f"Total Deposits: ${stats['total_deposits']:.2f}")
        print(f"Total Fees: ${stats['total_fees']:.2f}")
        print(f"Current Bankroll: ${stats['current_bankroll']:.2f}")
        print(f"Net Profit/Loss: ${stats['total_profit_loss']:.2f}")
        
        # Add up/down indicator
        if stats['total_profit_loss'] > 0:
            print(f"Status: UP ${stats['total_profit_loss']:.2f}")
        elif stats['total_profit_loss'] < 0:
            print(f"Status: DOWN ${abs(stats['total_profit_loss']):.2f}")
        else:
            print("Status: BREAK EVEN")
        
        if stats['total_sessions'] > 0:
            print(f"Average per Session: ${stats['average_session']:.2f}")
        print("="*40)
    
    @staticmethod
    def show_recent_sessions(sessions, num_sessions=10):
        recent = sessions[-num_sessions:]
        print(f"\nLast {len(recent)} Sessions:")
        print("-" * 70)
        for i, session in enumerate(recent, 1):
            date = datetime.datetime.fromisoformat(session['date']).strftime('%m/%d %H:%M')
            profit_loss = session['profit_loss']
            sign = "+" if profit_loss > 0 else ""
            
            # Handle both old and new session formats
            if 'new_deposit' in session:
                deposit_info = f"(+${session['new_deposit']:.2f})" if session['new_deposit'] > 0 else "(cont.)"
            else:
                deposit_info = f"(+${session['buy_in']:.2f})"
            
            print(f"{i:2d}. {date} | {deposit_info} | Out: ${session['cash_out']:6.2f} | {sign}${profit_loss:6.2f}")