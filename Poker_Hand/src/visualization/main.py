import matplotlib.pyplot as plt
import matplotlib.patches as patches
import sys
import os

# Add the data folder to the Python path
data_path = r"C:\Users\cinco\Desktop\Poker_Analysis\Trainer\data"
sys.path.append(data_path)

# Now import from the data folder
from hand_ranges import HandRanges
from chart_data import ChartData

# We'll avoid importing position_filters since it has relative import issues
# Instead, we'll recreate the needed functionality here

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

def convert_hands_to_chart_format(hands_dict):
    """
    Convert hand ranges from your data format to chart visualization format.
    Args:
        hands_dict: Dictionary with 'paired', 'suited', 'unsuited' keys
    Returns:
        set: Formatted hands for chart visualization
    """
    chart_hands = set()
    
    # Add paired hands (e.g., "A-A" becomes "AA")
    if 'paired' in hands_dict:
        for hand in hands_dict['paired']:
            chart_hands.add(hand.replace('-', ''))
    
    # Add suited hands (e.g., "A-K" becomes "AKs")
    if 'suited' in hands_dict:
        for hand in hands_dict['suited']:
            chart_hands.add(hand.replace('-', '') + 's')
    
    # Add unsuited hands (e.g., "A-K" becomes "AKo")
    if 'unsuited' in hands_dict:
        for hand in hands_dict['unsuited']:
            chart_hands.add(hand.replace('-', '') + 'o')
    
    return chart_hands

def get_all_position_hands(position):
    """
    Get all hands for a specific position from Chart 1.
    """
    all_hands = set()
    chart_1 = HandRanges.get_chart_1_hands()
    
    if position == "any_position":
        hands_dict = chart_1.get("any_position", {})
        all_hands.update(convert_hands_to_chart_format(hands_dict))
    elif position == "mid_late_position":
        # Include any_position + mid_late_position hands
        for pos in ["any_position", "mid_late_position"]:
            hands_dict = chart_1.get(pos, {})
            all_hands.update(convert_hands_to_chart_format(hands_dict))
    elif position == "late_position":
        # Include all positions
        for pos in ["any_position", "mid_late_position", "late_position"]:
            hands_dict = chart_1.get(pos, {})
            all_hands.update(convert_hands_to_chart_format(hands_dict))
    
    return all_hands

def convert_chart2_hands_to_format(hand_list):
    """
    Convert Chart 2 hands to visualization format.
    Assumes all hands are both suited and unsuited unless they're pairs.
    """
    chart_hands = set()
    
    for hand in hand_list:
        if hand[0] == hand[2]:  # It's a pair (e.g., "A-A")
            chart_hands.add(hand.replace('-', ''))
        else:  # It's a non-pair, add both suited and unsuited
            base_hand = hand.replace('-', '')
            chart_hands.add(base_hand + 's')
            chart_hands.add(base_hand + 'o')
    
    return chart_hands

if __name__ == '__main__':
    print("Loading poker hand ranges from data folder...")
    
    try:
        # --- Example 1: Chart 1 - Any Position Range ---
        print("Creating Chart 1: Any Position...")
        any_position_range = get_all_position_hands("any_position")
        create_range_chart(any_position_range, "Chart 1: Any Position")
        
        # --- Example 2: Chart 1 - Mid/Late Position Range ---
        print("Creating Chart 1: Mid/Late Position...")
        mid_late_range = get_all_position_hands("mid_late_position")
        create_range_chart(mid_late_range, "Chart 1: Mid/Late Position")
        
        # --- Example 3: Chart 1 - Late Position Range ---
        print("Creating Chart 1: Late Position...")
        late_position_range = get_all_position_hands("late_position")
        create_range_chart(late_position_range, "Chart 1: Late Position")
        
        # --- Example 4: Chart 2 - Playable Hands ---
        print("Creating Chart 2: Playable Hands...")
        chart2_playable = ChartData.get_playable_hands()
        playable_range = convert_chart2_hands_to_format(chart2_playable)
        create_range_chart(playable_range, "Chart 2: Playable Hands")
        
        # --- Example 5: Chart 2 - All Positive Hands (Playable + Marginal) ---
        print("Creating Chart 2: All Positive Hands...")
        all_positive = ChartData.get_playable_hands() + ChartData.get_marginal_hands()
        positive_range = convert_chart2_hands_to_format(all_positive)
        create_range_chart(positive_range, "Chart 2: All Positive Hands")
        
        # --- Example 6: Custom range using your original example ---
        print("Creating Custom EP1 Open Shoves...")
        ep1_open_shoves_range = {
            # Pocket Pairs
            'AA', 'KK', 'QQ', 'JJ', 'TT', '99', '88', '77', '66', '55',
            # Suited Aces
            'AKs', 'AQs', 'AJs', 'ATs', 'A9s', 'A8s', 'A7s', 'A6s', 'A5s', 'A4s', 'A3s',
            # Suited Kings
            'KQs', 'KJs', 'KTs', 'K9s',
            # Suited Queens
            'QJs', 'QTs',
            # Other Suited Connectors
            'JTs',
            # Offsuit Hands
            'AKo', 'AQo', 'AJo', 'KQo'
        }
        create_range_chart(ep1_open_shoves_range, "EP1 Open Shoves")
        
        print("All charts generated successfully!")
        
    except Exception as e:
        print(f"Error occurred: {e}")
        print("Make sure matplotlib is installed: pip install matplotlib")