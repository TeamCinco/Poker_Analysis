import datetime

class Display:
    @staticmethod
    def show_stats(stats):
        print("\n" + "="*40)
        print("POKER TRACKER STATS")
        print("="*40)
        print(f"Total Sessions: {stats['total_sessions']}")
        print(f"Total Money In: ${stats['total_buy_ins']:.2f}")
        print(f"Total Money Out: ${stats['total_cash_outs']:.2f}")
        print(f"Total Fees: ${stats['total_fees']:.2f}")
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
        print("-" * 60)
        for i, session in enumerate(recent, 1):
            date = datetime.datetime.fromisoformat(session['date']).strftime('%m/%d %H:%M')
            profit_loss = session['profit_loss']
            sign = "+" if profit_loss > 0 else ""
            print(f"{i:2d}. {date} | In: ${session['buy_in']:6.2f} | Out: ${session['cash_out']:6.2f} | {sign}${profit_loss:6.2f}")