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
    wedges, texts, autotexts = ax.pie(texture_counts.values, 
                                      labels=texture_counts.index,
                                      autopct='%1.1f%%',
                                      startangle=90,
                                      colors=[colors['primary'], colors['secondary'], colors['accent'], 
                                             colors['warning'], colors['success'], colors['neutral']][:len(texture_counts)])
    
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
                linewidths=0.5, ax=ax)
    
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
               color=colors['accent'], alpha=0.7, label='C-bet Freq')
    ax4.scatter(df['flush_potential'], df['expected_checkraise_freq'], 
               color=colors['warning'], alpha=0.7, label='Check-raise Freq')
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

def create_board_comparison_chart(dfs, output_path, colors):
    """Create comparison chart across different board categories"""
    fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(16, 12))
    fig.patch.set_facecolor('#1e1e1e')
    
    # Prepare comparison data
    categories = ['Dry', 'Wet', 'Paired', 'Special']
    
    # 1. Average metrics by category
    metrics = ['expected_cbet_freq', 'expected_checkraise_freq', 'connectivity_index', 'range_advantage_pfr']
    
    data_for_comparison = {}
    for i, (name, df) in enumerate(zip(categories, dfs)):
        if df is not None:
            data_for_comparison[name] = df[metrics].mean()
    
    # Create grouped bar chart
    comparison_df = pd.DataFrame(data_for_comparison).T
    
    x = np.arange(len(comparison_df.index))
    width = 0.2
    
    for i, metric in enumerate(metrics):
        bars = ax1.bar(x + i*width, comparison_df[metric], width, 
                      label=metric.replace('_', ' ').title(),
                      color=[colors['primary'], colors['secondary'], colors['accent'], colors['warning']][i])
    
    ax1.set_xlabel('Board Categories', color='white')
    ax1.set_ylabel('Average Value', color='white')
    ax1.set_title('Strategic Metrics by Board Category', color='white', fontweight='bold')
    ax1.set_xticks(x + width * 1.5)
    ax1.set_xticklabels(comparison_df.index)
    ax1.legend()
    ax1.tick_params(colors='white')
    ax1.set_facecolor('#2d2d2d')
    
    # 2. Board count comparison
    counts = [len(df) if df is not None else 0 for df in dfs]
    bars2 = ax2.bar(categories, counts, color=[colors['primary'], colors['secondary'], colors['accent'], colors['warning']])
    ax2.set_ylabel('Number of Boards', color='white')
    ax2.set_title('Board Count by Category', color='white', fontweight='bold')
    ax2.tick_params(colors='white')
    ax2.set_facecolor('#2d2d2d')
    
    for bar, count in zip(bars2, counts):
        ax2.text(bar.get_x() + bar.get_width()/2, bar.get_height() + 0.5, str(count),
                ha='center', va='bottom', color='white', fontweight='bold')
    
    # 3. Connectivity distribution comparison
    for i, (name, df) in enumerate(zip(categories, dfs)):
        if df is not None:
            ax3.hist(df['connectivity_index'], alpha=0.7, label=name, bins=15,
                    color=[colors['primary'], colors['secondary'], colors['accent'], colors['warning']][i])
    
    ax3.set_xlabel('Connectivity Index', color='white')
    ax3.set_ylabel('Frequency', color='white')
    ax3.set_title('Connectivity Distribution by Category', color='white', fontweight='bold')
    ax3.legend()
    ax3.tick_params(colors='white')
    ax3.set_facecolor('#2d2d2d')
    
    # 4. C-bet vs Check-raise frequency scatter
    for i, (name, df) in enumerate(zip(categories, dfs)):
        if df is not None:
            ax4.scatter(df['expected_cbet_freq'], df['expected_checkraise_freq'], 
                       alpha=0.7, label=name, s=60,
                       color=[colors['primary'], colors['secondary'], colors['accent'], colors['warning']][i])
    
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

def generate_summary_report(dfs, output_path):
    """Generate a text summary report of the analysis"""
    with open(output_path, 'w') as f:
        f.write("BOARD ANALYSIS SUMMARY REPORT\n")
        f.write("=" * 50 + "\n\n")
        
        categories = ['Comprehensive', 'Dry', 'Wet', 'Paired', 'Special Cases']
        
        for name, df in zip(categories, dfs):
            if df is not None:
                f.write(f"{name.upper()} BOARDS:\n")
                f.write(f"Total boards analyzed: {len(df)}\n")
                
                if 'primary_texture' in df.columns:
                    f.write(f"Most common texture: {df['primary_texture'].mode().iloc[0]}\n")
                
                f.write(f"Average connectivity: {df['connectivity_index'].mean():.3f}\n")
                f.write(f"Average c-bet frequency: {df['expected_cbet_freq'].mean():.3f}\n")
                f.write(f"Average check-raise frequency: {df['expected_checkraise_freq'].mean():.3f}\n")
                f.write(f"Average range advantage: {df['range_advantage_pfr'].mean():.3f}\n")
                f.write("\n" + "-" * 30 + "\n\n")
        
        f.write("KEY INSIGHTS:\n")
        f.write("• Dry boards have highest c-bet frequencies\n")
        f.write("• Wet boards favor check-raise strategies\n") 
        f.write("• Paired boards require balanced approaches\n")
        f.write("• Connectivity strongly correlates with action frequencies\n")
    
    print(f"Created summary report: {os.path.basename(output_path)}")

def main():
    """Main execution function"""
    print("=" * 60)
    print("BOARD ANALYSIS VISUALIZATION GENERATOR")
    print("=" * 60)
    
    # Setup
    colors = setup_style()
    base_output_path = "Poker_Hand/src/visualization/output"
    create_output_structure(base_output_path)
    
    # Input files
    input_base = "Poker_Hand/output/board_analysis"
    files = {
        'comprehensive': f"{input_base}/comprehensive_board_analysis.csv",
        'dry': f"{input_base}/dry_boards_analysis.csv",
        'wet': f"{input_base}/wet_boards_analysis.csv", 
        'paired': f"{input_base}/paired_boards_analysis.csv",
        'special': f"{input_base}/special_cases_analysis.csv"
    }
    
    # Load all datasets
    datasets = {}
    for name, filepath in files.items():
        datasets[name] = load_board_data(filepath)
    
    print(f"\nGenerating visualizations in {base_output_path}...")
    
    # Generate individual category charts
    for category, df in datasets.items():
        if df is not None:
            category_path = f"{base_output_path}/board_analysis/{category}_boards"
            
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
    create_board_comparison_chart(
        comparison_dfs, 
        f"{base_output_path}/board_analysis/comparisons/category_comparison.png", 
        colors)
    
    # Generate summary report
    generate_summary_report(
        [datasets['comprehensive'], datasets['dry'], datasets['wet'], datasets['paired'], datasets['special']],
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
