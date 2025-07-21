import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
import seaborn as sns
import os
from pathlib import Path

def setup_style():
    """Set up matplotlib style for professional-looking charts"""
    plt.style.use('dark_background')
    sns.set_palette("husl")
    
    # Custom color palette for poker analysis
    colors = {
        'primary': '#FF6B6B',     # Red
        'secondary': '#4ECDC4',   # Teal  
        'accent': '#45B7D1',      # Blue
        'warning': '#FFA07A',     # Orange
        'success': '#98D8C8',     # Green
        'neutral': '#6C7B95'      # Grey
    }
    return colors

def create_output_structure(base_path):
    """Create organized output directory structure"""
    directories = [
        'board_analysis/comprehensive',
        'board_analysis/dry_boards',
        'board_analysis/wet_boards', 
        'board_analysis/paired_boards',
        'board_analysis/special_cases',
        'board_analysis/comparisons'
    ]
    
    for directory in directories:
        os.makedirs(f"{base_path}/{directory}", exist_ok=True)
    
    return base_path

def load_board_data(csv_path):
    """Load and validate board analysis CSV data"""
    try:
        df = pd.read_csv(csv_path)
        print(f"Loaded {len(df)} boards from {os.path.basename(csv_path)}")
        print(f"  - Columns: {list(df.columns)}")
        if 'primary_texture' in df.columns:
            print(f"  - Textures: {df['primary_texture'].unique()}")
        return df
    except Exception as e:
        print(f"Error loading {csv_path}: {e}")
        return None

def create_texture_distribution_chart(df, title, output_path, colors):
    """Create a pie chart showing board texture distribution"""
    fig, ax = plt.subplots(figsize=(12, 8))
    fig.patch.set_facecolor('#1e1e1e')
    
    # Count primary textures
    texture_counts = df['primary_texture'].value_counts()
    
    # Create pie chart with custom colors
    color_list = [colors['primary'], colors['secondary'], colors['accent'], 
                  colors['warning'], colors['success'], colors['neutral']]
    chart_colors = color_list[:len(texture_counts)]
    
    wedges, texts, autotexts = ax.pie(texture_counts.values, 
                                      labels=texture_counts.index,
                                      autopct='%1.1f%%',
                                      startangle=90,
                                      colors=chart_colors)
    
    # Enhance text appearance
    for autotext in autotexts:
        autotext.set_color('white')
        autotext.set_fontweight('bold')
        autotext.set_fontsize(12)
    
    for text in texts:
        text.set_color('white')
        text.set_fontsize(11)
    
    ax.set_title(title, color='white', fontsize=18, fontweight='bold', pad=20)
    
    plt.tight_layout()
    plt.savefig(output_path, dpi=300, bbox_inches='tight', facecolor='#1e1e1e')
    plt.close()
    print(f"Created: {os.path.basename(output_path)}")

def create_connectivity_analysis(df, title, output_path, colors):
    """Create connectivity index analysis chart"""
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(16, 8))
    fig.patch.set_facecolor('#1e1e1e')
    
    # Histogram of connectivity indices
    ax1.hist(df['connectivity_index'], bins=20, color=colors['accent'], alpha=0.7, edgecolor='white')
    ax1.set_xlabel('Connectivity Index', color='white', fontsize=12)
    ax1.set_ylabel('Number of Boards', color='white', fontsize=12)
    ax1.set_title('Distribution of Board Connectivity', color='white', fontsize=14, fontweight='bold')
    ax1.tick_params(colors='white')
    
    # Box plot by texture type
    texture_order = df['primary_texture'].value_counts().index
    sns.boxplot(data=df, y='primary_texture', x='connectivity_index', 
                order=texture_order, ax=ax2, palette='husl')
    ax2.set_xlabel('Connectivity Index', color='white', fontsize=12)
    ax2.set_ylabel('Board Texture', color='white', fontsize=12)
    ax2.set_title('Connectivity by Texture Type', color='white', fontsize=14, fontweight='bold')
    ax2.tick_params(colors='white')
    
    # Style the axes
    for ax in [ax1, ax2]:
        ax.set_facecolor('#2d2d2d')
        for spine in ax.spines.values():
            spine.set_color('white')
    
    plt.suptitle(title, color='white', fontsize=18, fontweight='bold', y=0.95)
    plt.tight_layout()
    plt.savefig(output_path, dpi=300, bbox_inches='tight', facecolor='#1e1e1e')
    plt.close()
    print(f"Created: {os.path.basename(output_path)}")

def create_strategic_frequency_heatmap(df, title, output_path, colors):
    """Create heatmap of strategic frequencies by board texture"""
    fig, ax = plt.subplots(figsize=(14, 10))
    fig.patch.set_facecolor('#1e1e1e')
    
    # Prepare data for heatmap
    strategic_data = df.groupby('primary_texture').agg({
        'expected_cbet_freq': 'mean',
        'expected_checkraise_freq': 'mean',
        'range_advantage_pfr': 'mean',
        'connectivity_index': 'mean',
        'flush_potential': 'mean',
        'pair_potential': 'mean',
        'high_card_bias': 'mean'
    }).round(3)
    
    # Create heatmap
    sns.heatmap(strategic_data.T, annot=True, cmap='RdYlBu_r', 
                cbar_kws={'label': 'Frequency/Index Value'},
                linewidths=0.5, ax=ax, fmt='.3f')
    
    ax.set_title(title, color='white', fontsize=18, fontweight='bold', pad=20)
    ax.set_xlabel('Board Texture', color='white', fontsize=12)
    ax.set_ylabel('Strategic Metrics', color='white', fontsize=12)
    ax.tick_params(colors='white')
    
    # Color the colorbar
    cbar = ax.collections[0].colorbar
    cbar.ax.tick_params(colors='white')
    cbar.set_label('Frequency/Index Value', color='white')
    
    plt.xticks(rotation=45, ha='right')
    plt.tight_layout()
    plt.savefig(output_path, dpi=300, bbox_inches='tight', facecolor='#1e1e1e')
    plt.close()
    print(f"Created: {os.path.basename(output_path)}")

def create_c_bet_frequency_analysis(df, title, output_path, colors):
    """Create detailed C-bet frequency analysis"""
    fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(16, 12))
    fig.patch.set_facecolor('#1e1e1e')
    
    # 1. C-bet frequency by texture
    texture_cbet = df.groupby('primary_texture')['expected_cbet_freq'].mean().sort_values(ascending=True)
    bars1 = ax1.barh(texture_cbet.index, texture_cbet.values, color=colors['primary'])
    ax1.set_xlabel('Average C-bet Frequency', color='white')
    ax1.set_title('C-bet Frequency by Board Texture', color='white', fontweight='bold')
    ax1.tick_params(colors='white')
    ax1.set_facecolor('#2d2d2d')
    
    # Add value labels on bars
    for bar, value in zip(bars1, texture_cbet.values):
        ax1.text(value + 0.01, bar.get_y() + bar.get_height()/2, f'{value:.2f}', 
                va='center', color='white', fontweight='bold')
    
    # 2. Check-raise frequency by texture  
    texture_cr = df.groupby('primary_texture')['expected_checkraise_freq'].mean().sort_values(ascending=True)
    bars2 = ax2.barh(texture_cr.index, texture_cr.values, color=colors['secondary'])
    ax2.set_xlabel('Average Check-raise Frequency', color='white')
    ax2.set_title('Check-raise Frequency by Texture', color='white', fontweight='bold')
    ax2.tick_params(colors='white')
    ax2.set_facecolor('#2d2d2d')
    
    for bar, value in zip(bars2, texture_cr.values):
        ax2.text(value + 0.005, bar.get_y() + bar.get_height()/2, f'{value:.3f}', 
                va='center', color='white', fontweight='bold')
    
    # 3. Range advantage scatter plot
    scatter = ax3.scatter(df['connectivity_index'], df['range_advantage_pfr'], 
                         c=df['expected_cbet_freq'], cmap='plasma', alpha=0.7, s=60)
    ax3.set_xlabel('Connectivity Index', color='white')
    ax3.set_ylabel('Range Advantage PFR', color='white')
    ax3.set_title('Range Advantage vs Connectivity', color='white', fontweight='bold')
    ax3.tick_params(colors='white')
    ax3.set_facecolor('#2d2d2d')
    
    # Add colorbar
    cbar = plt.colorbar(scatter, ax=ax3)
    cbar.set_label('C-bet Frequency', color='white')
    cbar.ax.tick_params(colors='white')
    
    # 4. Flush potential vs frequencies
    ax4.scatter(df['flush_potential'], df['expected_cbet_freq'], 
               color=colors['accent'], alpha=0.7, label='C-bet Freq', s=60)
    ax4.scatter(df['flush_potential'], df['expected_checkraise_freq'], 
               color=colors['warning'], alpha=0.7, label='Check-raise Freq', s=60)
    ax4.set_xlabel('Flush Potential', color='white')
    ax4.set_ylabel('Frequency', color='white') 
    ax4.set_title('Frequencies vs Flush Potential', color='white', fontweight='bold')
    ax4.legend()
    ax4.tick_params(colors='white')
    ax4.set_facecolor('#2d2d2d')
    
    # Style all axes
    for ax in [ax1, ax2, ax3, ax4]:
        for spine in ax.spines.values():
            spine.set_color('white')
    
    plt.suptitle(title, color='white', fontsize=18, fontweight='bold', y=0.95)
    plt.tight_layout()
    plt.savefig(output_path, dpi=300, bbox_inches='tight', facecolor='#1e1e1e')
    plt.close()
    print(f"Created: {os.path.basename(output_path)}")

def create_board_comparison_chart(dfs, labels, output_path, colors):
    """Create comparison chart across different board categories"""
    fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(16, 12))
    fig.patch.set_facecolor('#1e1e1e')
    
    # Filter out None dataframes
    valid_data = [(label, df) for label, df in zip(labels, dfs) if df is not None and len(df) > 0]
    
    if len(valid_data) == 0:
        print("No valid data for comparison chart")
        return
    
    valid_labels, valid_dfs = zip(*valid_data)
    
    # 1. Average metrics by category
    metrics = ['expected_cbet_freq', 'expected_checkraise_freq', 'connectivity_index', 'range_advantage_pfr']
    
    data_for_comparison = {}
    for label, df in valid_data:
        data_for_comparison[label] = df[metrics].mean()
    
    # Create grouped bar chart
    comparison_df = pd.DataFrame(data_for_comparison).T
    
    x = np.arange(len(comparison_df.index))
    width = 0.2
    
    color_list = [colors['primary'], colors['secondary'], colors['accent'], colors['warning']]
    
    for i, metric in enumerate(metrics):
        bars = ax1.bar(x + i*width, comparison_df[metric], width, 
                      label=metric.replace('_', ' ').title(),
                      color=color_list[i])
    
    ax1.set_xlabel('Board Categories', color='white')
    ax1.set_ylabel('Average Value', color='white')
    ax1.set_title('Strategic Metrics by Board Category', color='white', fontweight='bold')
    ax1.set_xticks(x + width * 1.5)
    ax1.set_xticklabels(comparison_df.index, rotation=45, ha='right')
    ax1.legend()
    ax1.tick_params(colors='white')
    ax1.set_facecolor('#2d2d2d')
    
    # 2. Board count comparison
    counts = [len(df) for _, df in valid_data]
    bars2 = ax2.bar(valid_labels, counts, color=color_list[:len(valid_labels)])
    ax2.set_ylabel('Number of Boards', color='white')
    ax2.set_title('Board Count by Category', color='white', fontweight='bold')
    ax2.tick_params(colors='white')
    ax2.set_facecolor('#2d2d2d')
    
    for bar, count in zip(bars2, counts):
        ax2.text(bar.get_x() + bar.get_width()/2, bar.get_height() + 0.5, str(count),
                ha='center', va='bottom', color='white', fontweight='bold')
    
    # 3. Connectivity distribution comparison
    for i, (label, df) in enumerate(valid_data):
        ax3.hist(df['connectivity_index'], alpha=0.7, label=label, bins=15,
                color=color_list[i % len(color_list)])
    
    ax3.set_xlabel('Connectivity Index', color='white')
    ax3.set_ylabel('Frequency', color='white')
    ax3.set_title('Connectivity Distribution by Category', color='white', fontweight='bold')
    ax3.legend()
    ax3.tick_params(colors='white')
    ax3.set_facecolor('#2d2d2d')
    
    # 4. C-bet vs Check-raise frequency scatter
    for i, (label, df) in enumerate(valid_data):
        ax4.scatter(df['expected_cbet_freq'], df['expected_checkraise_freq'], 
                   alpha=0.7, label=label, s=60,
                   color=color_list[i % len(color_list)])
    
    ax4.set_xlabel('C-bet Frequency', color='white')
    ax4.set_ylabel('Check-raise Frequency', color='white')
    ax4.set_title('C-bet vs Check-raise Frequencies', color='white', fontweight='bold')
    ax4.legend()
    ax4.tick_params(colors='white')
    ax4.set_facecolor('#2d2d2d')
    
    # Style all axes
    for ax in [ax1, ax2, ax3, ax4]:
        for spine in ax.spines.values():
            spine.set_color('white')
    
    plt.suptitle('Board Category Analysis Comparison', color='white', fontsize=18, fontweight='bold', y=0.95)
    plt.tight_layout()
    plt.savefig(output_path, dpi=300, bbox_inches='tight', facecolor='#1e1e1e')
    plt.close()
    print(f"Created: {os.path.basename(output_path)}")

def generate_summary_report(datasets, output_path):
    """Generate a text summary report of the analysis"""
    with open(output_path, 'w') as f:
        f.write("BOARD ANALYSIS SUMMARY REPORT\n")
        f.write("=" * 50 + "\n\n")
        
        for name, df in datasets.items():
            if df is not None and len(df) > 0:
                f.write(f"{name.upper().replace('_', ' ')} BOARDS:\n")
                f.write(f"Total boards analyzed: {len(df)}\n")
                
                if 'primary_texture' in df.columns:
                    textures = df['primary_texture'].value_counts()
                    f.write(f"Most common texture: {textures.index[0]} ({textures.iloc[0]} boards)\n")
                    f.write(f"Texture distribution: {dict(textures)}\n")
                
                f.write(f"Average connectivity: {df['connectivity_index'].mean():.3f}\n")
                f.write(f"Average c-bet frequency: {df['expected_cbet_freq'].mean():.3f}\n")
                f.write(f"Average check-raise frequency: {df['expected_checkraise_freq'].mean():.3f}\n")
                f.write(f"Average range advantage: {df['range_advantage_pfr'].mean():.3f}\n")
                f.write(f"Average flush potential: {df['flush_potential'].mean():.3f}\n")
                f.write(f"Average pair potential: {df['pair_potential'].mean():.3f}\n")
                f.write("\n" + "-" * 30 + "\n\n")
        
        f.write("KEY INSIGHTS:\n")
        f.write("• Highly Connected boards show lower c-bet frequencies\n")
        f.write("• Paired boards have higher c-bet frequencies due to range advantage\n")
        f.write("• Monotone boards favor check-raise strategies\n")
        f.write("• Connectivity strongly correlates with action frequencies\n")
        f.write("• High card bias affects range advantages significantly\n")
    
    print(f"Created summary report: {os.path.basename(output_path)}")

def main():
    """Main execution function"""
    print("=" * 60)
    print("BOARD ANALYSIS VISUALIZATION GENERATOR")
    print("=" * 60)
    
    # Setup
    colors = setup_style()
    
    # Updated paths
    base_output_path = r"C:\Users\cinco\Desktop\Poker_Analysis\Poker_Hand\src\visualization\output"
    input_base_path = r"C:\Users\cinco\Desktop\Poker_Analysis\Poker_Hand\build\output\board_analysis"
    
    create_output_structure(base_output_path)
    
    # Input files with full paths
    files = {
        "comprehensive": os.path.join(input_base_path, "comprehensive_board_analysis.csv"),
        "dry": os.path.join(input_base_path, "dry_boards_analysis.csv"),
        "wet": os.path.join(input_base_path, "wet_boards_analysis.csv"),
        "paired": os.path.join(input_base_path, "paired_boards_analysis.csv"),
        "special": os.path.join(input_base_path, "special_cases_analysis.csv")
    }
    
    # Load all datasets
    datasets = {}
    for name, filepath in files.items():
        datasets[name] = load_board_data(filepath)
    
    print(f"\nGenerating visualizations in {base_output_path}...")
    
    # Generate individual category charts
    for category, df in datasets.items():
        if df is not None and len(df) > 0:
            # Create category-specific folder
            if category == "comprehensive":
                category_path = f"{base_output_path}/board_analysis/comprehensive"
            else:
                category_path = f"{base_output_path}/board_analysis/{category}_boards"
            
            # Ensure the directory exists
            os.makedirs(category_path, exist_ok=True)
            
            print(f"\nGenerating charts for {category} category...")
            
            # Texture distribution
            create_texture_distribution_chart(
                df, f"{category.title()} Board Texture Distribution",
                f"{category_path}/texture_distribution.png", colors)
            
            # Connectivity analysis  
            create_connectivity_analysis(
                df, f"{category.title()} Board Connectivity Analysis",
                f"{category_path}/connectivity_analysis.png", colors)
            
            # Strategic frequency heatmap
            create_strategic_frequency_heatmap(
                df, f"{category.title()} Strategic Frequency Analysis", 
                f"{category_path}/strategic_heatmap.png", colors)
            
            # C-bet analysis
            create_c_bet_frequency_analysis(
                df, f"{category.title()} C-bet Analysis",
                f"{category_path}/cbet_analysis.png", colors)
    
    # Generate comparison charts
    comparison_dfs = [datasets['dry'], datasets['wet'], datasets['paired'], datasets['special']]
    comparison_labels = ['Dry', 'Wet', 'Paired', 'Special']
    
    create_board_comparison_chart(
        comparison_dfs, comparison_labels,
        f"{base_output_path}/board_analysis/comparisons/category_comparison.png", 
        colors)
    
    # Generate summary report
    generate_summary_report(
        datasets,
        f"{base_output_path}/board_analysis/summary_report.txt")
    
    print("\n" + "=" * 60)
    print("VISUALIZATION GENERATION COMPLETE!")
    print("=" * 60)
    print(f"Output location: {base_output_path}/board_analysis/")
    print("\nGenerated files:")
    print("📊 Texture distribution charts")
    print("📈 Connectivity analysis charts") 
    print("🔥 Strategic frequency heatmaps")
    print("💰 C-bet analysis dashboards")
    print("⚖️  Category comparison charts")
    print("📄 Summary report")
    print("\nReady for poker strategy analysis!")

if __name__ == "__main__":
    main()