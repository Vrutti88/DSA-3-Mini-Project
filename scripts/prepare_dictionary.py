#!/usr/bin/env python3
"""
Prepares the dictionary datasets for Smart Autocomplete & Typo Correction.
Sources legitimate English words from system dictionary (/usr/share/dict/words),
augments with curated Computer Science & Programming terms, normalizes to lowercase,
removes duplicates/invalid characters, and assigns deterministic simulated frequency values
modeled using a standard Zipf-like rank-frequency distribution.
"""

import os
import re
import math

# Curated high-priority programming & CS terms with explicitly assigned frequencies
CURATED_CS_TERMS = {
    # Requested specific examples
    "program": 900,
    "progress": 700,
    "programming": 500,
    "programmer": 300,
    "programmable": 150,
    "python": 950,
    "javascript": 920,
    "java": 910,
    "computer": 880,
    "computing": 650,
    "computation": 520,
    "computational": 410,
    "apple": 870,
    "application": 860,
    "apply": 640,
    "applicable": 420,
    "machine": 850,
    "learning": 840,
    "artificial": 810,
    "intelligence": 800,
    "algorithm": 830,
    "data": 980,
    "database": 890,
    "development": 820,
    "developer": 780,
    "develop": 720,
    "software": 870,
    "hardware": 760,
    "network": 850,
    "networking": 620,
    "security": 840,
    "secure": 610,
    "function": 830,
    "functional": 580,
    "variable": 800,
    "structure": 810,
    "structural": 490,
    "tree": 790,
    "trie": 750,
    "graph": 780,
    "graphics": 590,
    "search": 880,
    "searching": 630,
    "sorting": 770,
    "sort": 710,
    
    # Common CS keywords & vocabulary
    "binary": 720,
    "boolean": 680,
    "pointer": 650,
    "recursion": 670,
    "recursive": 610,
    "dynamic": 740,
    "distance": 690,
    "matrix": 700,
    "array": 860,
    "queue": 730,
    "stack": 790,
    "heap": 710,
    "hash": 780,
    "table": 820,
    "vector": 770,
    "string": 890,
    "character": 750,
    "memory": 820,
    "allocation": 620,
    "compiler": 740,
    "compilation": 590,
    "compile": 680,
    "debug": 700,
    "debugger": 630,
    "debugging": 650,
    "backend": 760,
    "frontend": 760,
    "fullstack": 640,
    "server": 840,
    "client": 830,
    "protocol": 720,
    "interface": 810,
    "object": 850,
    "class": 870,
    "inheritance": 640,
    "polymorphism": 580,
    "encapsulation": 570,
    "abstraction": 610,
    "complexity": 750,
    "asymptotic": 540,
    "constant": 680,
    "linear": 720,
    "logarithmic": 660,
    "quadratic": 590,
    "exponential": 580,
    "optimization": 710,
    "optimize": 670,
    "performance": 810,
    "benchmark": 730,
    "iteration": 650,
    "iterator": 620,
    "traverse": 640,
    "traversal": 660,
    "breadth": 620,
    "depth": 690,
    "levenshtein": 650,
    "autocomplete": 820,
    "correction": 710,
    "dictionary": 790,
    "system": 890,
    "process": 830,
    "thread": 770,
    "concurrency": 640,
    "parallel": 680,
    "operating": 760,
    "framework": 780,
    "library": 800,
    "package": 770,
    "module": 760,
    "project": 820,
    "architecture": 740,
    "terminal": 720,
    "command": 790,
    "execution": 710,
    "execute": 690,
    "runtime": 750,
    "template": 730,
    "pointer": 710,
    "reference": 750
}

def load_system_words(limit=60000):
    words = []
    seen = set()
    
    # 1. Add curated terms first
    for term, freq in CURATED_CS_TERMS.items():
        w = term.lower().strip()
        if w not in seen:
            seen.add(w)
            words.append((w, freq))
            
    # 2. Add words from /usr/share/dict/words
    dict_path = "/usr/share/dict/words"
    if os.path.exists(dict_path):
        with open(dict_path, "r", encoding="utf-8", errors="ignore") as f:
            for line in f:
                w = line.strip().lower()
                # filter: alphabetic only, length between 2 and 24
                if re.fullmatch(r"[a-z]{2,24}", w) and w not in seen:
                    seen.add(w)
                    # Assign deterministic Zipf-based frequency
                    # Rank r starts after curated terms
                    r = len(words) + 1
                    # Frequency between 5 and 500
                    freq = max(5, int(1500 / (r ** 0.45)))
                    words.append((w, freq))
                    if len(words) >= limit:
                        break
                        
    return words

def main():
    os.makedirs("data", exist_ok=True)
    all_words = load_system_words(60000)
    print(f"Loaded total {len(all_words)} unique sanitized words.")
    
    # Slices to write
    slices = [
        ("data/dict_1k.txt", 1000),
        ("data/dict_5k.txt", 5000),
        ("data/dict_10k.txt", 10000),
        ("data/dict_25k.txt", 25000),
        ("data/dict_50k.txt", 50000),
        ("data/dictionary.txt", 10000) # Default primary dictionary
    ]
    
    for filepath, count in slices:
        subset = all_words[:count]
        with open(filepath, "w", encoding="utf-8") as f:
            for w, freq in subset:
                f.write(f"{w} {freq}\n")
        print(f"Wrote {len(subset)} entries to {filepath}")

if __name__ == "__main__":
    main()
