#!/usr/bin/env python3
"""
Poker Strategy Data Visualization Script

This script generates comprehensive visualizations from poker strategy CSV files,
creating professional-quality charts with poker-appropriate color schemes.

Author: AI Assistant
Date: 2025
Python Version: 3.11.1
Dependencies: pandas, matplotlib, seaborn, numpy
"""

import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np
import os
from pathlib import Path
from typing import Dict, List, Tuple

# Set up matplotlib and seaborn styling
plt.style.use('default')
sns.set_palette("husl")

# Define poker-appropriate color schemes
POKER_COLORS = {
    'raise': '#2E8B57',      # Sea Green - aggressive action
    'call': '#4169E1',       # Royal Blue - passive action  
    'open': '#FFD700',       # Gold - opening action
    '3bet': '#FF8C00',       # Dark Orange - 3-bet action
    '4bet': '#8A2BE2',       # Blue Violet - 4-bet action
    'fold': '#DC143C',       # Crimson - fold action
    'check': '#708090'       # Slate Gray - check action
}

# Color gradient for EV visualization (red to green)
EV_COLORMAP = 'RdYlGn'

def load_csv_data(file_path: str) -> pd.DataFrame:
    """
    Load CSV data from the specified file path.
    
    Args:
        file_path (str): Path to the CSV file
        
    Returns:
        pd.DataFrame: Loaded data
        
    Raises:
        FileNotFoundError: If the file doesn't exist
        pd.errors.EmptyDataError: If the file is empty
    """
    try:
        df = pd.read_csv(file_path)
        print(f"✓ Successfully loaded {len(df)} rows from {os.path.basename(file_path)}")
        return df
    except FileNotFoundError:
        print(f"✗ Error: File not found - {file_path}")
        raise
    except pd.errors.EmptyDataError:
        print(f"✗ Error: Empty file - {file_path}")
        raise
    except Exception as e:
        print(f"✗ Error loading {file_path}: {str(e)}")
        raise

def create_ev_by_hand_chart(df: pd.DataFrame, output_path: str, filename_prefix: str) -> None:
    """
    Create a horizontal bar chart showing average Expected Value by hand.
    
    Args:
        df (pd.DataFrame): Input data
        output_path (str): Directory to save the chart
        filename_prefix (str): Prefix for the output filename
    """
    # Calculate average EV by hand
    hand_ev = df.groupby('hand')['expected_value'].mean().sort_values(ascending=True)
    
    # Create figure with appropriate size
    plt.figure(figsize=(12, max(8, len(hand_ev) * 0.3)))
    
    # Create horizontal bar chart with color gradient
    bars = plt.barh(range(len(hand_ev)), hand_ev.values)
    
    # Apply color gradient based on EV values
    norm = plt.Normalize(vmin=hand_ev.min(), vmax=hand_ev.max())
    colors = plt.cm.RdYlGn(norm(hand_ev.values))
    
    for bar, color in zip(bars, colors):
        bar.set_color(color)
    
    # Customize the chart
    plt.yticks(range(len(hand_ev)), hand_ev.index, fontsize=10)
    plt.xlabel('Average Expected Value (EV)', fontsize=12, fontweight='bold')
    plt.ylabel('Hand', fontsize=12, fontweight='bold')
    plt.title('Average Expected Value (EV) by Hand', fontsize=16, fontweight='bold', pad=20)
    
    # Add value labels on bars
    for i, (hand, ev) in enumerate(hand_ev.items()):
        plt.text(ev + (hand_ev.max() - hand_ev.min()) * 0.01, i, f'{ev:.3f}', 
                va='center', fontsize=9, fontweight='bold')
    
    # Add grid for better readability
    plt.grid(axis='x', alpha=0.3, linestyle='--')
    
    # Adjust layout and save
    plt.tight_layout()
    save_path = os.path.join(output_path, f"{filename_prefix}_ev_by_hand.png")
    plt.savefig(save_path, dpi=300, bbox_inches='tight', facecolor='white')
    plt.close()
    print(f"✓ Saved: {save_path}")

def create_decision_by_position_chart(df: pd.DataFrame, output_path: str, filename_prefix: str) -> None:
    """
    Create a grouped bar chart showing decision frequency by position.
    
    Args:
        df (pd.DataFrame): Input data
        output_path (str): Directory to save the chart
        filename_prefix (str): Prefix for the output filename
    """
    # Count decisions by position and decision type
    decision_counts = df.groupby(['position', 'decision_type']).size().unstack(fill_value=0)
    
    # Create figure
    plt.figure(figsize=(14, 8))
    
    # Create grouped bar chart
    ax = decision_counts.plot(kind='bar', 
                             color=[POKER_COLORS.get(col, '#1f77b4') for col in decision_counts.columns],
                             width=0.8)
    
    # Customize the chart
    plt.title('Frequency of Decisions by Position', fontsize=16, fontweight='bold', pad=20)
    plt.xlabel('Position', fontsize=12, fontweight='bold')
    plt.ylabel('Count of Decisions', fontsize=12, fontweight='bold')
    plt.xticks(rotation=45, ha='right')
    
    # Customize legend
    plt.legend(title='Decision Type', title_fontsize=12, fontsize=10, 
              bbox_to_anchor=(1.05, 1), loc='upper left')
    
    # Add value labels on bars
    for container in ax.containers:
        ax.bar_label(container, fontsize=8, rotation=90, padding=3)
    
    # Add grid for better readability
    plt.grid(axis='y', alpha=0.3, linestyle='--')
    
    # Adjust layout and save
    plt.tight_layout()
    save_path = os.path.join(output_path, f"{filename_prefix}_decision_by_position.png")
    plt.savefig(save_path, dpi=300, bbox_inches='tight', facecolor='white')
    plt.close()
    print(f"✓ Saved: {save_path}")

def create_ev_distribution_chart(df: pd.DataFrame, output_path: str, filename_prefix: str) -> None:
    """
    Create a histogram with KDE overlay showing Expected Value distribution.
    
    Args:
        df (pd.DataFrame): Input data
        output_path (str): Directory to save the chart
        filename_prefix (str): Prefix for the output filename
    """
    # Create figure
    plt.figure(figsize=(12, 8))
    
    # Create histogram with KDE overlay
    ax = plt.gca()
    
    # Plot histogram
    n, bins, patches = plt.hist(df['expected_value'], bins=50, alpha=0.7, 
                               color='lightblue', edgecolor='black', linewidth=0.5)
    
    # Color histogram bars with gradient
    norm = plt.Normalize(vmin=bins.min(), vmax=bins.max())
    for patch, bin_val in zip(patches, bins[:-1]):
        patch.set_facecolor(plt.cm.RdYlGn(norm(bin_val)))
    
    # Add KDE line
    ax2 = ax.twinx()
    df['expected_value'].plot.kde(ax=ax2, color='darkred', linewidth=3, label='KDE')
    ax2.set_ylabel('Density (KDE)', fontsize=12, fontweight='bold')
    ax2.legend(loc='upper right')
    
    # Customize the chart
    plt.sca(ax)  # Set current axis back to histogram
    plt.xlabel('Expected Value (EV)', fontsize=12, fontweight='bold')
    plt.ylabel('Frequency', fontsize=12, fontweight='bold')
    plt.title('Distribution of Expected Value (EV)', fontsize=16, fontweight='bold', pad=20)
    
    # Add statistics text box
    mean_ev = df['expected_value'].mean()
    std_ev = df['expected_value'].std()
    median_ev = df['expected_value'].median()
    
    stats_text = f'Mean: {mean_ev:.3f}\nStd: {std_ev:.3f}\nMedian: {median_ev:.3f}'
    plt.text(0.02, 0.98, stats_text, transform=ax.transAxes, fontsize=10,
            verticalalignment='top', bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.8))
    
    # Add grid for better readability
    plt.grid(axis='y', alpha=0.3, linestyle='--')
    
    # Adjust layout and save
    plt.tight_layout()
    save_path = os.path.join(output_path, f"{filename_prefix}_ev_distribution.png")
    plt.savefig(save_path, dpi=300, bbox_inches='tight', facecolor='white')
    plt.close()
    print(f"✓ Saved: {save_path}")

def create_ev_by_board_chart(df: pd.DataFrame, output_path: str) -> None:
    """
    Create a box plot showing Expected Value across different flop boards.
    This function is specifically for the dynamic_flop_strategy.csv file.
    
    Args:
        df (pd.DataFrame): Input data (should contain 'board' column)
        output_path (str): Directory to save the chart
    """
    if 'board' not in df.columns or df['board'].isna().all():
        print("⚠ Warning: No board data found, skipping board texture visualization")
        return
    
    # Filter out rows with empty board values
    df_with_boards = df[df['board'].notna() & (df['board'] != '')]
    
    if len(df_with_boards) == 0:
        print("⚠ Warning: No valid board data found, skipping board texture visualization")
        return
    
    # Get unique boards and limit to most common ones for readability
    board_counts = df_with_boards['board'].value_counts()
    top_boards = board_counts.head(20).index.tolist()  # Top 20 most common boards
    
    df_filtered = df_with_boards[df_with_boards['board'].isin(top_boards)]
    
    # Create figure
    plt.figure(figsize=(16, 10))
    
    # Create box plot
    box_plot = plt.boxplot([df_filtered[df_filtered['board'] == board]['expected_value'].values 
                           for board in top_boards],
                          labels=top_boards,
                          patch_artist=True,
                          showfliers=True)
    
    # Color the boxes with a gradient
    colors = plt.cm.Set3(np.linspace(0, 1, len(top_boards)))
    for patch, color in zip(box_plot['boxes'], colors):
        patch.set_facecolor(color)
        patch.set_alpha(0.7)
    
    # Customize the chart
    plt.title('Expected Value Across Different Flop Boards', fontsize=16, fontweight='bold', pad=20)
    plt.xlabel('Flop Board', fontsize=12, fontweight='bold')
    plt.ylabel('Expected Value (EV)', fontsize=12, fontweight='bold')
    plt.xticks(rotation=45, ha='right', fontsize=10)
    
    # Add grid for better readability
    plt.grid(axis='y', alpha=0.3, linestyle='--')
    
    # Adjust layout and save
    plt.tight_layout()
    save_path = os.path.join(output_path, "dynamic_flop_strategy_ev_by_board.png")
    plt.savefig(save_path, dpi=300, bbox_inches='tight', facecolor='white')
    plt.close()
    print(f"✓ Saved: {save_path}")

def ensure_output_directory(output_path: str) -> None:
    """
    Create output directory if it doesn't exist.
    
    Args:
        output_path (str): Path to the output directory
    """
    Path(output_path).mkdir(parents=True, exist_ok=True)
    print(f"✓ Output directory ready: {output_path}")

def process_csv_file(file_path: str, output_path: str) -> None:
    """
    Process a single CSV file and generate all applicable visualizations.
    
    Args:
        file_path (str): Path to the CSV file
        output_path (str): Directory to save visualizations
    """
    filename = os.path.basename(file_path)
    filename_prefix = os.path.splitext(filename)[0]
    
    print(f"\n📊 Processing: {filename}")
    print("=" * 50)
    
    try:
        # Load data
        df = load_csv_data(file_path)
        
        if df.empty:
            print(f"⚠ Warning: {filename} is empty, skipping...")
            return
        
        # Display basic info about the dataset
        print(f"📈 Dataset Info:")
        print(f"   • Rows: {len(df):,}")
        print(f"   • Columns: {len(df.columns)}")
        print(f"   • EV Range: {df['expected_value'].min():.3f} to {df['expected_value'].max():.3f}")
        
        # Generate standard visualizations for all files
        create_ev_by_hand_chart(df, output_path, filename_prefix)
        create_decision_by_position_chart(df, output_path, filename_prefix)
        create_ev_distribution_chart(df, output_path, filename_prefix)
        
        # Generate board-specific visualization for flop strategy file
        if 'flop' in filename.lower() and 'board' in df.columns:
            create_ev_by_board_chart(df, output_path)
        
        print(f"✅ Completed processing: {filename}")
        
    except Exception as e:
        print(f"❌ Error processing {filename}: {str(e)}")

def main() -> None:
    """
    Main function to orchestrate the visualization generation process.
    """
    print("🎯 Poker Strategy Data Visualization Generator")
    print("=" * 60)
    
    # Define paths
    input_dir = "/Users/jazzhashzzz/Desktop/Poker_Analysis/hands/src/visualization/input/hands/"
    output_dir = "/Users/jazzhashzzz/Desktop/Poker_Analysis/hands/src/visualization/output/"
    
    # CSV files to process
    csv_files = [
        "dynamic_preflop_strategy.csv",
        "dynamic_flop_strategy.csv", 
        "profitable_strategies_only.csv"
    ]
    
    # Ensure output directory exists
    ensure_output_directory(output_dir)
    
    # Process each CSV file
    total_files = len(csv_files)
    for i, csv_file in enumerate(csv_files, 1):
        file_path = os.path.join(input_dir, csv_file)
        
        print(f"\n🔄 Progress: {i}/{total_files}")
        
        if not os.path.exists(file_path):
            print(f"⚠ Warning: File not found - {csv_file}")
            continue
            
        process_csv_file(file_path, output_dir)
    
    print(f"\n🎉 Visualization generation complete!")
    print(f"📁 All charts saved to: {output_dir}")
    print("\n📊 Generated visualizations:")
    print("   • Average Expected Value by Hand (horizontal bar charts)")
    print("   • Decision Frequency by Position (grouped bar charts)")  
    print("   • Expected Value Distribution (histograms with KDE)")
    print("   • Expected Value by Board Texture (box plot - flop data only)")
    print("\n💡 All charts are saved as high-resolution PNG files (300 DPI)")

if __name__ == "__main__":
    main()
