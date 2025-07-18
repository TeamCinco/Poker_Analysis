from poker_tracker import PokerTracker
from display import Display
from quant_display import QuantDisplay
from alpha_display import AlphaDisplay

class Menu:
    def __init__(self):
        self.tracker = PokerTracker()
        self.display = Display()
    
    def show_menu(self):
        current_bankroll = self.tracker.get_current_bankroll()
        print(f"\nCurrent Bankroll: ${current_bankroll:.2f}")
        print("\n1. Add session")
        print("2. View stats")
        print("3. View recent sessions")
        print("4. Add your current sessions")
        print("5. Quantitative Analysis")
        print("6. Performance Breakdown")
        print("7. 🚨 Alpha Decay Analysis")
        print("8. 📊 Performance Alerts")
        print("9. Exit")
    
    def handle_add_session(self):
        try:
            current_bankroll = self.tracker.get_current_bankroll()
            print(f"Current bankroll: ${current_bankroll:.2f}")
            print("Enter 0 for buy-in to continue with current bankroll")
            
            buy_in = float(input("Buy-in amount (0 to continue): $"))
            cash_out = float(input("Cash-out amount: $"))
            fees = float(input("Fees (enter 0 if none): $"))
            notes = input("Notes (optional): ")
            self.tracker.add_session(buy_in, cash_out, fees, notes)
        except ValueError:
            print("Invalid input. Please enter numbers.")
    
    def handle_view_stats(self):
        stats = self.tracker.get_stats()
        self.display.show_stats(stats)
    
    def handle_recent_sessions(self):
        try:
            num = int(input("How many recent sessions? (default 10): ") or "10")
            self.display.show_recent_sessions(self.tracker.sessions, num)
        except ValueError:
            self.display.show_recent_sessions(self.tracker.sessions)
    
    def handle_add_current_sessions(self):
        print("Adding your two sessions...")
        self.tracker.add_session(50, 9.27, 6.50, "First session - lost big")
        self.tracker.add_session(27, 56.11, 0, "Second session - won back some")
        print("Sessions added!")
    
    def handle_quant_analysis(self):
        QuantDisplay.show_quant_analysis(self.tracker.sessions)
    
    def handle_performance_breakdown(self):
        QuantDisplay.show_performance_breakdown(self.tracker.sessions)
    
    def handle_alpha_decay_analysis(self):
        AlphaDisplay.show_alpha_decay_analysis(self.tracker.sessions)
    
    def handle_performance_alerts(self):
        AlphaDisplay.show_performance_alerts(self.tracker.sessions)
    
    def run(self):
        while True:
            self.show_menu()
            choice = input("\nChoose option (1-9): ").strip()
            
            if choice == "1":
                self.handle_add_session()
            elif choice == "2":
                self.handle_view_stats()
            elif choice == "3":
                self.handle_recent_sessions()
            elif choice == "4":
                self.handle_add_current_sessions()
            elif choice == "5":
                self.handle_quant_analysis()
            elif choice == "6":
                self.handle_performance_breakdown()
            elif choice == "7":
                self.handle_alpha_decay_analysis()
            elif choice == "8":
                self.handle_performance_alerts()
            elif choice == "9":
                print("Good luck at the tables!")
                break
            else:
                print("Invalid choice.")