#!/usr/bin/env python3
"""
Generates academic publication-quality benchmark charts from C++ measurements.
"""

import os
import pandas as pd
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt

def generate_graphs():
    csv_path = "benchmarks/results/benchmark_results.csv"
    if not os.path.exists(csv_path):
        print(f"Error: {csv_path} not found. Run benchmark first.")
        return

    df = pd.read_csv(csv_path)
    os.makedirs("benchmarks/graphs", exist_ok=True)

    # Style configuration
    plt.style.use('seaborn-v0_8-whitegrid' if 'seaborn-v0_8-whitegrid' in plt.style.available else 'default')
    plt.rcParams.update({
        'font.sans-serif': 'DejaVu Sans',
        'font.size': 11,
        'axes.labelsize': 12,
        'axes.titlesize': 14,
        'xtick.labelsize': 10,
        'ytick.labelsize': 10,
        'legend.fontsize': 11,
        'figure.titlesize': 16
    })

    # GRAPH 1: Trie vs Linear Search Execution Time
    fig, ax = plt.subplots(figsize=(8, 5), dpi=300)
    ax.plot(df['dataset_size'], df['linear_search_avg_us'], 'o-', color='#e53e3e', linewidth=2.5, markersize=8, label='Linear Dictionary Scan')
    ax.plot(df['dataset_size'], df['trie_search_avg_us'], 's-', color='#2b6cb0', linewidth=2.5, markersize=8, label='Trie Prefix Search')
    ax.set_title('Prefix Search Execution Time: Trie vs Linear Scan', pad=15, weight='bold')
    ax.set_xlabel('Dictionary Size (Number of Words)')
    ax.set_ylabel('Average Execution Time (Microseconds, μs)')
    ax.set_xticks(df['dataset_size'])
    ax.get_xaxis().set_major_formatter(plt.FuncFormatter(lambda x, p: f"{int(x):,}"))
    ax.legend(frameon=True, facecolor='white', framealpha=0.9)
    plt.tight_layout()
    graph1_path = "benchmarks/graphs/trie_vs_linear_search.png"
    plt.savefig(graph1_path)
    plt.close()
    print(f"Saved: {graph1_path}")

    # GRAPH 2: Execution Time against Dictionary Size (Prefix + Typo)
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 5), dpi=300)
    
    # Prefix subplot
    ax1.plot(df['dataset_size'], df['linear_search_avg_us'], 'o-', color='#e53e3e', label='Linear Scan (O(N·L))')
    ax1.plot(df['dataset_size'], df['trie_search_avg_us'], 's-', color='#2b6cb0', label='Trie (O(P + S))')
    ax1.set_title('Autocomplete Prefix Search Time', weight='bold')
    ax1.set_xlabel('Dictionary Size')
    ax1.set_ylabel('Execution Time (μs)')
    ax1.set_xticks(df['dataset_size'])
    ax1.get_xaxis().set_major_formatter(plt.FuncFormatter(lambda x, p: f"{int(x/1000)}k" if x >= 1000 else f"{int(x)}"))
    ax1.legend(frameon=True)

    # Typo subplot
    ax2.plot(df['dataset_size'], df['linear_typo_avg_us'], 'o-', color='#dd6b20', label='Linear All-Word DP (O(N·M·L))')
    ax2.plot(df['dataset_size'], df['trie_typo_avg_us'], '^-', color='#38a169', label='Trie Branch-Pruned DP')
    ax2.set_title('Typo Correction Time', weight='bold')
    ax2.set_xlabel('Dictionary Size')
    ax2.set_ylabel('Execution Time (μs)')
    ax2.set_xticks(df['dataset_size'])
    ax2.get_xaxis().set_major_formatter(plt.FuncFormatter(lambda x, p: f"{int(x/1000)}k" if x >= 1000 else f"{int(x)}"))
    ax2.legend(frameon=True)

    plt.tight_layout()
    graph2_path = "benchmarks/graphs/execution_time_vs_dict_size.png"
    plt.savefig(graph2_path)
    plt.close()
    print(f"Saved: {graph2_path}")

    # GRAPH 3: Performance Speedup Comparison
    fig, ax = plt.subplots(figsize=(8, 5), dpi=300)
    bars1 = ax.bar([x - 1200 for x in df['dataset_size']], df['search_speedup'], width=2400, color='#3182ce', label='Prefix Search Speedup')
    bars2 = ax.bar([x + 1200 for x in df['dataset_size']], df['typo_speedup'], width=2400, color='#38a169', label='Typo Correction Speedup')
    
    ax.set_title('Algorithmic Speedup Factor (Linear Time / Trie Time)', pad=15, weight='bold')
    ax.set_xlabel('Dictionary Size')
    ax.set_ylabel('Speedup Factor (x Times Faster)')
    ax.set_xticks(df['dataset_size'])
    ax.get_xaxis().set_major_formatter(plt.FuncFormatter(lambda x, p: f"{int(x):,}"))
    ax.legend(frameon=True, facecolor='white', framealpha=0.9)

    # Add labels on top of bars
    for bar in bars1:
        yval = bar.get_height()
        ax.text(bar.get_x() + bar.get_width()/2.0, yval + 1, f"{yval:.1f}x", ha='center', va='bottom', fontsize=8, weight='bold')
    for bar in bars2:
        yval = bar.get_height()
        ax.text(bar.get_x() + bar.get_width()/2.0, yval + 1, f"{yval:.1f}x", ha='center', va='bottom', fontsize=8, weight='bold')

    ax.set_ylim(0, max(df['search_speedup'].max(), df['typo_speedup'].max()) * 1.15)
    plt.tight_layout()
    graph3_path = "benchmarks/graphs/performance_speedup.png"
    plt.savefig(graph3_path)
    plt.close()
    print(f"Saved: {graph3_path}")

if __name__ == "__main__":
    generate_graphs()
