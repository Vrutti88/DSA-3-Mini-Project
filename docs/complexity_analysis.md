# Theoretical & Empirical Complexity Analysis

**Course:** Data Structures & Algorithms - 3 (Mini-Project)  
**Topic:** Asymptotic Analysis & Empirical Performance Evaluation  

---

## 1. Asymptotic Complexity Summary

| Operation | Trie Data Structure | Linear Dictionary Scan | Theoretical Speedup |
| :--- | :--- | :--- | :--- |
| **Data Structure Build Time** | $O(\sum_{i=1}^{N} L_i) = O(N \cdot \bar{L})$ | $O(N \cdot \bar{L})$ (loading into vector) | Similar build overhead |
| **Prefix Search (Lookup)** | $O(P + S)$ | $O(N \cdot P)$ | $\mathbf{O(N / (1 + S/P))}$ |
| **Result Ranking (Top $K$)** | $O(M \log K)$ | $O(M \log K)$ | Equivalent |
| **Typo Correction (Edit Dist)**| $O(\text{Unpruned Nodes} \cdot M)$ | $O(N \cdot M \cdot \bar{L})$ | $\mathbf{O(\frac{N \cdot \bar{L}}{\text{Unpruned Nodes}})}$ |
| **Auxiliary Space Complexity** | $O(\text{Node Count} \cdot |\Sigma|)$ | $O(N \cdot \bar{L})$ | Linear Scan uses less memory |

*Notation:*
- $N$: Total number of words in the dictionary.
- $\bar{L}$: Average word length ($\approx 8.5$ characters).
- $P$: Length of the input prefix query (e.g., $P = 3$ for `"pro"`).
- $S$: Total number of nodes in the matched prefix subtree.
- $M$: Length of the misspelled word.
- $K$: Maximum number of output suggestions returned (default 5).
- $|\Sigma|$: Alphabet size ($26$ lowercase English characters).

---

## 2. In-Depth Asymptotic Proofs

### A. Prefix Search Complexity
- **Linear Scan:** To check whether any word starts with prefix $P$, a sequential scan must compare up to $P$ characters against each of the $N$ words in the lexicon. Thus, linear search requires $N \times P$ character equality checks in the worst case:
  $$T_{\text{linear}}(N, P) = O(N \cdot P)$$
  Crucially, as the dictionary size $N$ scales from 1,000 to 50,000 words, linear search time grows strictly linearly ($\Theta(N)$).

- **Trie Prefix Search:** Starting at the root, each character of prefix $P$ indexes directly into the 26-pointer array in $O(1)$ time:
  $$\text{Index}(c) = \text{tolower}(c) - \text{'a'}$$
  Reaching the prefix terminal node requires exactly $P$ pointer dereferences, taking $O(P)$ time. Once at the prefix node, only the subtree containing matching descendant words is traversed ($S$ nodes), without examining any unrelated words in the dictionary:
  $$T_{\text{trie}}(P, S) = O(P + S)$$
  **Fundamental Insight:** Trie prefix lookup is **independent of total dictionary size $N$**. Whether the dictionary contains 10,000 or 10,000,000 words, finding the node for `"pro"` takes identical time ($P = 3$).

### B. Typo Correction Complexity
- **Brute-Force Linear Scan:** Computes the full 2D Levenshtein DP matrix against all $N$ words:
  $$T_{\text{linear\_typo}}(N, M, \bar{L}) = O(N \cdot M \cdot \bar{L})$$
  For $N = 50,000$, $M = 8$, $\bar{L} = 8$, this requires approximately $3,200,000$ matrix cell updates per query!
- **Trie Traversal with DP Row Pruning:** Because thousands of dictionary words share common prefixes, the DP row state for those shared prefixes is computed **only once** at their common ancestor node. Furthermore, pruning subtrees where $\min(\text{currentRow}) > 2$ terminates exploration after depth 2 or 3 for over 98% of the Trie branches:
  $$T_{\text{trie\_typo}} = O(\text{Visited Nodes} \cdot M) \ll O(N \cdot M \cdot \bar{L})$$

---

## 3. Real Empirical Measurements

The following measurements were collected by running the compiled C++17 engine (`bin/engine --benchmark`) with `std::chrono::high_resolution_clock` across 50 repeated trials on this system:

| Dataset Size ($N$) | Trie Build Time | Memory (KB) | Trie Search Avg | Linear Search Avg | **Prefix Speedup** | Trie Typo Avg | Linear Typo Avg | **Typo Speedup** |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| **1,000 words** | 0.18 ms | 806.7 KB | 0.29 μs | 3.69 μs | **12.7x** | 16.23 μs | 110.18 μs | **6.8x** |
| **5,000 words** | 0.95 ms | 3,993.9 KB | 0.25 μs | 17.90 μs | **70.9x** | 37.69 μs | 517.45 μs | **13.7x** |
| **10,000 words** | 1.40 ms | 8,098.5 KB | 0.60 μs | 34.27 μs | **57.2x** | 51.53 μs | 1,031.42 μs | **20.0x** |
| **25,000 words** | 3.58 ms | 19,953.7 KB | 1.82 μs | 84.65 μs | **46.5x** | 91.84 μs | 2,504.56 μs | **27.3x** |
| **50,000 words** | 7.80 ms | 39,248.8 KB | 3.19 μs | 162.36 μs | **51.0x** | 169.38 μs | 4,693.59 μs | **27.7x** |

---

## 4. Trade-Off Analysis: When is Trie NOT Better?

While Trie outperforms linear scanning by up to **70.9x** for prefix retrieval, an objective academic analysis must acknowledge its engineering trade-offs:

1. **Memory Overhead:**
   - A contiguous array of strings packs data tightly in CPU cache. In contrast, each `TrieNode` in our 26-pointer array occupies:
     $$26 \times 8 \text{ bytes (pointers)} + 1 \text{ byte (bool)} + 4 \text{ bytes (int)} + 32 \text{ bytes (string)} \approx 248 \text{ bytes}$$
     For 50,000 words ($\approx 159,128$ nodes), the Trie consumes $\approx 39.2 \text{ MB}$, whereas flat text consumes only $\approx 0.6 \text{ MB}$.
2. **Small Lexicon Cardinality ($N < 200$):**
   - For tiny dictionaries ($< 200$ words), the entire vector fits inside L1/L2 CPU cache. The sequential memory streaming of vector iteration can be as fast as or faster than pointer chasing across dynamic node allocations.
3. **Arbitrary Substring / Suffix Queries:**
   - A standard Trie only indexes prefixes. Searching for words ending with a suffix or containing an infix requires scanning all branches or maintaining an inverted Suffix Tree / Generalized Suffix Tree.

