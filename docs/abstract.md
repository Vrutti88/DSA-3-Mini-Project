# Academic Project Abstract

**Project Title:** Smart Autocomplete and Typo Correction with Frequency-Ranked Retrieval  
**Course Code:** CS302 / DSA-3 (Data Structures & Algorithms Laboratory)  
**Academic Degree:** Bachelor of Technology (B.Tech) in Computer Science & Engineering  
**Evaluation Scheme:** 100 Marks Capstone Mini-Project  

---

## Abstract

In modern information retrieval systems, search-as-you-type autocomplete and resilient typo correction are fundamental requirements for seamless human-computer interaction. Traditional naive approaches rely on sequentially scanning exhaustive dictionary arrays ($O(N \cdot L)$), which suffers from severe latency scaling bottlenecks as lexicon cardinality grows into tens of thousands of entries. Furthermore, brute-force spell checking requires computing dynamic programming edit distances against every individual dictionary word ($O(N \cdot M \cdot L)$), resulting in intolerable per-keystroke latencies.

This project designs, engineers, and rigorously benchmarks an enterprise-grade, memory-efficient **Smart Autocomplete and Typo Correction System** implemented from scratch in **C++17**, wrapped in a lightweight **Python Flask** REST API and presented via a high-performance, responsive **HTML5/CSS3/ES6** web application.

The core algorithmic architecture leverages two foundational computer science data structures and paradigms:
1. **Trie (Prefix Tree):** An $m$-ary character tree providing $O(P)$ prefix lookup time complexity where $P$ is the prefix query length, strictly independent of the total dictionary size $N$. Terminal nodes store corpus frequency data, enabling $O(K \log K)$ descending frequency ranking with lexicographical tie-breaking for the top-$K$ recommendations.
2. **Trie-Integrated Levenshtein Dynamic Programming with Branch Pruning:** Typo tolerance is achieved not by brute-force string comparisons, but by propagating DP distance rows down the Trie branches during Depth-First Search (DFS). By establishing that the edit distance between any dictionary branch and the misspelled target cannot decrease below the minimum value in the active DP row, the algorithm prunes subtrees whenever $\min(\text{currentRow}) > K$ (where $K$ is the maximum allowable edit distance, default 2). This mathematical pruning skips over 98% of candidate word paths.

Empirical benchmarking across dictionary sizes ranging from 1,000 to 50,000 words reveals that the C++17 Trie implementation achieves an average prefix search latency of **0.29 μs to 3.18 μs**, delivering a **12.7x to 70.9x speedup** over linear scanning. Trie-based typo correction achieves latencies between **16.2 μs and 169.4 μs**, delivering up to a **27.7x speedup** over linear Levenshtein scanning. The project includes automated unit test suites, real-time head-to-head algorithm comparisons, interactive 2D DP matrix visualizers, and comprehensive academic documentation.

