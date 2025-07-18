from poker_tracker import PokerTracker
from display import Display

class Menu:
    def __init__(self):
        self.tracker = PokerTracker()
        self.display = Display()
    
    def show_menu(self):
        print("\n1. Add session")
        print("2. View stats")
        print("3. View recent sessions")
        print("4. Add your current sessions")
        print("5. Exit")
    
    def handle_add_session(self):
        try:
            buy_in = float(input("Buy-in amount: $"))
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
    
    def run(self):
        while True:
            self.show_menu()
            choice = input("\nChoose option (1-5): ").strip()
            
            if choice == "1":
                self.handle_add_session()
            elif choice == "2":
                self.handle_view_stats()
            elif choice == "3":
                self.handle_recent_sessions()
            elif choice == "4":
                self.handle_add_current_sessions()
            elif choice == "5":
                print("Good luck at the tables!")
                break
            else:
                print("Invalid choice.")