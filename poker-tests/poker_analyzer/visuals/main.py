import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np
from pathlib import Path
import warnings
warnings.filterwarnings('ignore')

# Set style
plt.style.use('seaborn-v0_8')
sns.set_palette("husl")

def load_data():
    """Load the poker analysis CSV data"""
    # Look for CSV in parent directory (build folder)
    csv_path = Path(r"C:\Users\cinco\Desktop\Poker_Analysis\poker-tests\poker_analyzer\build\poker_flop_analysis.csv")
    if not csv_path.exists():
        # Try current directory
        csv_path = Path("../poker_flop_analysis.csv")
    if not csv_path.exists():
        print("CSV file not found. Make sure to run the C++ analyzer first!")
        return None
    
    print(f"Loading data from: {csv_path}")
    df = pd.read_csv(csv_path)
    print(f"Loaded {len(df)} hands")
    return df

def parse_hand_info(df):
    """Extract useful information from hand strings"""
    df = df.copy()
    
    # Extract card ranks and suits
    df['card1_rank'] = df['Hand'].str[0]
    df['card1_suit'] = df['Hand'].str[1]
    df['card2_rank'] = df['Hand'].str[2]
    df['card2_suit'] = df['Hand'].str[3]
    
    # Determine if suited or offsuit
    df['suited'] = df['card1_suit'] == df['card2_suit']
    df['hand_type_category'] = df['suited'].map({True: 'Suited', False: 'Offsuit'})
    
    # Determine if pocket pair
    df['is_pocket_pair'] = df['card1_rank'] == df['card2_rank']
    df.loc[df['is_pocket_pair'], 'hand_type_category'] = 'Pocket Pair'
    
    # Create simplified hand notation (like AKs, AKo, AA)
    df['simplified_hand'] = df.apply(create_simplified_hand, axis=1)
    
    return df

def create_simplified_hand(row):
    """Create simplified hand notation like AKs, AKo, AA"""
    rank_order = 'AKQJT98765432A'  # A appears twice for wheel
    
    r1, r2 = row['card1_rank'], row['card2_rank']
    
    if r1 == r2:  # Pocket pair
        return f"{r1}{r2}"
    
    # Order by rank strength (A high)
    if rank_order.index(r1) < rank_order.index(r2):
        high, low = r1, r2
    else:
        high, low = r2, r1
    
    suffix = 's' if row['suited'] else 'o'
    return f"{high}{low}{suffix}"

def create_visualizations(df):
    """Create comprehensive visualizations"""
    
    # Create output directory
    output_dir = Path("./plots")
    output_dir.mkdir(exist_ok=True)
    
    fig = plt.figure(figsize=(20, 24))
    
    # 1. Hand Type Distribution
    plt.subplot(4, 3, 1)
    hand_type_counts = df['Hand_Type'].value_counts()
    plt.pie(hand_type_counts.values, labels=hand_type_counts.index, autopct='%1.1f%%')
    plt.title('Distribution of Best Hand Types After Flop', fontsize=14, fontweight='bold')
    
    # 2. Average Strength by Hand Type
    plt.subplot(4, 3, 2)
    avg_strength = df.groupby('Hand_Type')['Average_Strength'].mean().sort_values(ascending=True)
    plt.barh(avg_strength.index, avg_strength.values, color='skyblue')
    plt.title('Average Hand Strength by Type', fontsize=14, fontweight='bold')
    plt.xlabel('Average Strength')
    
    # 3. Suited vs Offsuit Performance
    plt.subplot(4, 3, 3)
    category_strength = df.groupby('hand_type_category')['Average_Strength'].mean()
    plt.bar(category_strength.index, category_strength.values, color=['red', 'green', 'blue'])
    plt.title('Average Strength: Suited vs Offsuit vs Pairs', fontsize=14, fontweight='bold')
    plt.ylabel('Average Strength')
    plt.xticks(rotation=45)
    
    # 4. Top 20 Starting Hands
    plt.subplot(4, 3, 4)
    top_hands = df.nlargest(20, 'Average_Strength')[['simplified_hand', 'Average_Strength']]
    plt.barh(top_hands['simplified_hand'], top_hands['Average_Strength'], color='gold')
    plt.title('Top 20 Starting Hands by Average Strength', fontsize=14, fontweight='bold')
    plt.xlabel('Average Strength')
    
    # 5. Bottom 20 Starting Hands
    plt.subplot(4, 3, 5)
    bottom_hands = df.nsmallest(20, 'Average_Strength')[['simplified_hand', 'Average_Strength']]
    plt.barh(bottom_hands['simplified_hand'], bottom_hands['Average_Strength'], color='lightcoral')
    plt.title('Bottom 20 Starting Hands by Average Strength', fontsize=14, fontweight='bold')
    plt.xlabel('Average Strength')
    
    # 6. Strength Distribution
    plt.subplot(4, 3, 6)
    plt.hist(df['Average_Strength'], bins=50, alpha=0.7, color='purple')
    plt.title('Distribution of Hand Strengths', fontsize=14, fontweight='bold')
    plt.xlabel('Average Strength')
    plt.ylabel('Frequency')
    
    # 7. Hand Type by Category
    plt.subplot(4, 3, 7)
    category_hand_type = pd.crosstab(df['hand_type_category'], df['Hand_Type'])
    category_hand_type.plot(kind='bar', stacked=True, ax=plt.gca())
    plt.title('Hand Types by Starting Hand Category', fontsize=14, fontweight='bold')
    plt.ylabel('Count')
    plt.xticks(rotation=45)
    plt.legend(bbox_to_anchor=(1.05, 1), loc='upper left')
    
    # 8. Pocket Pairs Analysis
    plt.subplot(4, 3, 8)
    pocket_pairs = df[df['is_pocket_pair']].copy()
    if len(pocket_pairs) > 0:
        pocket_pairs = pocket_pairs.sort_values('Average_Strength', ascending=False)
        plt.bar(pocket_pairs['simplified_hand'], pocket_pairs['Average_Strength'], color='orange')
        plt.title('Pocket Pairs Performance', fontsize=14, fontweight='bold')
        plt.ylabel('Average Strength')
        plt.xticks(rotation=45)
    
    # 9. Wins vs Ties Scatter
    plt.subplot(4, 3, 9)
    plt.scatter(df['Wins'], df['Ties'], alpha=0.6, c=df['Average_Strength'], cmap='viridis')
    plt.colorbar(label='Average Strength')
    plt.title('Wins vs Ties (colored by strength)', fontsize=14, fontweight='bold')
    plt.xlabel('Wins')
    plt.ylabel('Ties')
    
    # 10. Best Hand Type Frequency
    plt.subplot(4, 3, 10)
    hand_type_freq = df['Hand_Type'].value_counts()
    plt.bar(range(len(hand_type_freq)), hand_type_freq.values, color='teal')
    plt.title('Frequency of Each Best Hand Type', fontsize=14, fontweight='bold')
    plt.xlabel('Hand Type')
    plt.ylabel('Frequency')
    plt.xticks(range(len(hand_type_freq)), hand_type_freq.index, rotation=45)
    
    # 11. Strength by Suited Status
    plt.subplot(4, 3, 11)
    suited_data = []
    categories = ['Pocket Pair', 'Suited', 'Offsuit']
    for cat in categories:
        cat_data = df[df['hand_type_category'] == cat]['Average_Strength'].values
        suited_data.append(cat_data)
    
    plt.boxplot(suited_data, labels=categories)
    plt.title('Strength Distribution by Hand Category', fontsize=14, fontweight='bold')
    plt.ylabel('Average Strength')
    plt.xticks(rotation=45)
    
    # 12. Summary Statistics
    plt.subplot(4, 3, 12)
    plt.axis('off')
    
    # Calculate summary stats
    total_hands = len(df)
    avg_strength = df['Average_Strength'].mean()
    best_hand = df.loc[df['Average_Strength'].idxmax()]
    worst_hand = df.loc[df['Average_Strength'].idxmin()]
    
    stats_text = f"""
    POKER ANALYSIS SUMMARY
    =====================
    
    Total Hands Analyzed: {total_hands:,}
    Average Strength: {avg_strength:.2f}
    
    Best Hand: {best_hand['simplified_hand']}
    - Strength: {best_hand['Average_Strength']:.2f}
    - Best Result: {best_hand['Hand_Type']}
    
    Worst Hand: {worst_hand['simplified_hand']}
    - Strength: {worst_hand['Average_Strength']:.2f}
    - Best Result: {worst_hand['Hand_Type']}
    
    Hand Type Distribution:
    {df['Hand_Type'].value_counts().head().to_string()}
    """
    
    plt.text(0.1, 0.9, stats_text, transform=plt.gca().transAxes, 
             fontsize=10, verticalalignment='top', fontfamily='monospace')
    
    plt.tight_layout()
    plt.savefig(output_dir / 'poker_analysis_complete.png', dpi=300, bbox_inches='tight')
    plt.show()
    
    # Save summary data
    create_summary_reports(df, output_dir)

def create_summary_reports(df, output_dir):
    """Create text summary reports"""
    
    # Top hands report
    with open(output_dir / 'top_hands_report.txt', 'w') as f:
        f.write("TOP 50 STARTING HANDS BY AVERAGE STRENGTH\n")
        f.write("=" * 50 + "\n\n")
        
        top_50 = df.nlargest(50, 'Average_Strength')
        for i, (_, hand) in enumerate(top_50.iterrows(), 1):
            f.write(f"{i:2d}. {hand['simplified_hand']:4s} - Strength: {hand['Average_Strength']:7.2f} - Best: {hand['Hand_Type']}\n")
    
    # Category analysis
    with open(output_dir / 'category_analysis.txt', 'w') as f:
        f.write("HAND CATEGORY ANALYSIS\n")
        f.write("=" * 30 + "\n\n")
        
        for category in ['Pocket Pair', 'Suited', 'Offsuit']:
            cat_data = df[df['hand_type_category'] == category]
            f.write(f"{category.upper()}:\n")
            f.write(f"  Count: {len(cat_data)}\n")
            f.write(f"  Avg Strength: {cat_data['Average_Strength'].mean():.2f}\n")
            f.write(f"  Best Hand: {cat_data.loc[cat_data['Average_Strength'].idxmax(), 'simplified_hand']}\n")
            f.write(f"  Worst Hand: {cat_data.loc[cat_data['Average_Strength'].idxmin(), 'simplified_hand']}\n\n")
    
    print(f"Reports saved to {output_dir}/")

def main():
    print("Poker Flop Analysis Data Visualization")
    print("=" * 40)
    
    # Load data
    df = load_data()
    if df is None:
        return
    
    # Parse hand information
    df = parse_hand_info(df)
    
    # Create visualizations
    print("Creating visualizations...")
    create_visualizations(df)
    
    print("\nAnalysis complete! Check the 'plots' folder for results.")

if __name__ == "__main__":
    main()