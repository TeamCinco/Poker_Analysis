import matplotlib.pyplot as plt
import matplotlib.patches as patches
import pandas as pd
import os

def create_range_chart(range_hands, title):
    """
    Creates a poker hand range chart that visually highlights specific hands.
    Args:
        range_hands (set): A set of strings for the hands to highlight (e.g., {'AA', 'KK', 'AKs'}).
        title (str): The title to display above the chart.
    """
    # --- 1. Setup the Chart and Ranking Order ---
    ranks = ['A', 'K', 'Q', 'J', 'T', '9', '8', '7', '6', '5', '4', '3', '2']
    
    # Use a dark background for the figure for better contrast
    fig, ax = plt.subplots(figsize=(10, 10))
    fig.patch.set_facecolor('#2d2d2d')  # Dark grey background
    ax.set_facecolor('#2d2d2d')
    
    # --- 2. Iterate Through Grid and Draw Colored Cells ---
    for i, rank1 in enumerate(ranks):
        for j, rank2 in enumerate(ranks):
            # Determine the hand name based on its position in the grid
            if i == j:  # Pocket Pairs (diagonal)
                hand = rank1 + rank2
            elif j > i:  # Suited Hands (upper right)
                hand = rank1 + rank2 + 's'
            else:       # Offsuit Hands (lower left)
                hand = rank2 + rank1 + 'o'
            
            # Determine the color based on whether the hand is in the specified range
            is_in_range = hand in range_hands
            
            # Use specific hex colors to match the example image
            cell_color = '#d62f2f' if is_in_range else '#696969'  # Red or Grey
            text_color = 'white'
            
            # Draw the cell rectangle
            ax.add_patch(
                patches.Rectangle(
                    (j, 12 - i),  # (x, y)
                    1, 1,         # width, height
                    facecolor=cell_color,
                    edgecolor='#404040',  # Border color for cells
                    linewidth=2
                )
            )
            
            # Add the hand text in the center of the cell
            ax.text(j + 0.5, 12 - i + 0.5, hand,
                    ha='center', va='center',
                    color=text_color,
                    fontsize=14,
                    fontweight='bold')
    
    # --- 3. Finalize Chart Appearance ---
    ax.set_xlim(0, 13)
    ax.set_ylim(0, 13)
    ax.set_aspect('equal', adjustable='box')
    
    # Remove axis ticks and labels
    ax.set_xticks([])
    ax.set_yticks([])
    
    # Set the main title for the chart
    ax.set_title(title, color='white', fontsize=28, fontweight='bold', pad=20)
    
    plt.tight_layout()
    plt.show()

def load_range_from_csv(csv_path):
    """
    Load poker hand range from CSV file.
    Args:
        csv_path (str): Path to the CSV file
    Returns:
        set: Set of hands where in_range=True
    """
    try:
        df = pd.read_csv(csv_path)
        # Filter hands where in_range is True
        in_range_hands = df[df['in_range'] == True]['hand'].tolist()
        return set(in_range_hands)
    except Exception as e:
        print(f"Error loading CSV {csv_path}: {e}")
        return set()

if __name__ == '__main__':
    print("Loading poker hand ranges from CSV data...")
    
    # Base path to the output CSV files
    base_path = "Poker_Hand/output"
    
    try:
        # --- Example 1: UTG Opening Range (6-max) ---
        print("Creating UTG Opening Range (6-max)...")
        utg_range = load_range_from_csv(f"{base_path}/6_player/UTG/opening_raise/low_winrate_hands.csv")
        create_range_chart(utg_range, "UTG Opening Range (6-max)")
        
        # --- Example 2: Button Opening Range (6-max) ---
        print("Creating Button Opening Range (6-max)...")
        btn_range = load_range_from_csv(f"{base_path}/6_player/BTN/opening_raise/low_winrate_hands.csv")
        create_range_chart(btn_range, "Button Opening Range (6-max)")
        
        # --- Example 3: CO Opening Range (6-max) ---
        print("Creating CO Opening Range (6-max)...")
        co_range = load_range_from_csv(f"{base_path}/6_player/CO/opening_raise/low_winrate_hands.csv")
        create_range_chart(co_range, "CO Opening Range (6-max)")
        
        # --- Example 4: UTG 3-bet Range (6-max) ---
        print("Creating UTG 3-bet Range (6-max)...")
        utg_3bet_range = load_range_from_csv(f"{base_path}/6_player/UTG/3_bet/low_winrate_hands.csv")
        create_range_chart(utg_3bet_range, "UTG 3-bet Range (6-max)")
        
        # --- Example 5: Button 3-bet Range (6-max) ---
        print("Creating Button 3-bet Range (6-max)...")
        btn_3bet_range = load_range_from_csv(f"{base_path}/6_player/BTN/3_bet/low_winrate_hands.csv")
        create_range_chart(btn_3bet_range, "Button 3-bet Range (6-max)")
        
        print("All charts generated successfully!")
        print("Note: Charts show realistic poker ranges based on simulation data.")
        
    except Exception as e:
        print(f"Error occurred: {e}")
        print("Make sure pandas and matplotlib are installed: pip install pandas matplotlib")
