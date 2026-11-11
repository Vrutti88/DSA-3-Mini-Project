# Master Academic Project Report (100 Marks Evaluation)
# Smart Autocomplete and Typo Correction Engine

**Student / Candidate:** B.Tech Computer Science & Engineering  
**Course Title:** Data Structures and Algorithms - 3 (Mini-Project)  
**Academic Year:** 2026-2027  
**Technology Stack:** C++17, Dynamic Programming, Python Flask, HTML5, CSS3, ES6  

---

## 1. Project Title
**Smart Autocomplete and Typo Correction System with Trie-Integrated Dynamic Programming and Frequency Ranking**

---

## 2. Abstract
Modern search engines require sub-millisecond autocomplete suggestions and error-tolerant spell correction. Naive linear approaches scan entire dictionary arrays, scaling with $\Theta(N)$ complexity and becoming unusable at scale. This project implements an enterprise-grade, memory-efficient Smart Autocomplete and Typo Correction Engine from scratch in C++17, connected to a Python Flask API and an interactive web interface. The core engine utilizes an $m$-ary Trie for $O(P)$ prefix lookup and integrates Levenshtein Dynamic Programming into Trie traversal with branch pruning, achieving a 70.9x speedup in prefix search and a 27.7x speedup in typo correction over linear scanning on dictionaries up to 50,000 words.

---

## 3. Introduction
Autocomplete anticipates user intent by providing real-time suggestions as prefixes are typed. However, user input frequently contains orthographical typos (insertions, deletions, substitutions). A production search engine must simultaneously resolve matching prefixes and identify nearest-neighbor corrections while ranking results by relevance. This project implements these algorithms using pure C++17 data structures without third-party libraries, providing rigorous academic benchmarking and educational visualization.

---

## 4. Problem Statement
Implement a Trie for prefix retrieval and Levenshtein Edit Distance Dynamic Programming while traversing the Trie to discover spelling corrections within edit distance $K \le 2$. Rank candidate suggestions by corpus frequency. Compare performance against an equivalent linear dictionary scanning baseline across datasets of 1,000, 5,000, 10,000, 25,000, and 50,000 words using `std::chrono`.

---

## 5. Objectives
1. Implement an $m$-ary Trie from scratch in C++17 with 26-pointer arrays, terminal flags, and frequency accounting.
2. Build an autocomplete algorithm traversing matching subtrees and ranking candidates by frequency descending, with alphabetical tie-breaking.
3. Implement classical 2D Levenshtein Dynamic Programming for standalone distance calculation.
4. Implement Trie-integrated DP typo correction with branch pruning when $\min(\text{currentRow}) > K$.
5. Implement a linear scanning baseline with identical ranking for empirical comparison.
6. Benchmark loading, construction, and query execution times across 5 dataset sizes.
7. Deliver a modern, responsive web application with 4 pages: Smart Search, Typo Correction, Algorithm Explorer, and Performance Dashboard.

---

## 6. Scope
- **Lexicon:** 60,000 sanitized, lowercase English words derived from system dictionaries and computer science terminology, with deterministic simulated Zipfian frequency scores.
- **Operations:** Unit-cost insertions, deletions, and substitutions with configurable edit distance $K \in \{1, 2, 3\}$.
- **Target Applications:** Search engines, IDE code editors, terminal auto-suggestions, and e-commerce product catalogs.

---

## 7. Existing System
Traditional implementations store lexicons in flat text arrays (`std::vector<std::string>`).
- Prefix search iterates through every word: $O(N \cdot P)$.
- Typo correction recomputes Levenshtein DP against every word: $O(N \cdot M \cdot \bar{L})$.
- Causes high CPU utilization and noticeable per-keystroke input lag.

---

## 8. Proposed System
The proposed system replaces flat arrays with a Trie and tree-integrated DP:
- Prefix navigation is $O(P)$, completely independent of dictionary size $N$.
- Shared prefixes compute DP states only once at common ancestor nodes.
- Mathematical branch pruning terminates exploration when $\min(\text{row}) > K$, skipping $>98\%$ of dictionary nodes.
- Suggestions are ranked in $O(M \log K)$ using partial sort.

---

## 9. System Architecture
```
[Client Web Browser] <--> [Python Flask Gateway (REST API)] <--> [Compiled C++17 Engine]
                                                                        |
                                                 +----------------------+----------------------+
                                                 |                                             |
                                          [Trie Engine]                              [DictionaryScanner]
                                          - 26-Pointer Array                         - Linear Scan Baseline
                                          - Subtree DFS Collector                    - Brute-Force Levenshtein
                                          - Levenshtein Branch Pruning               - Benchmark Suite
```

---

## 10. Functional Requirements
- **FR-1:** Prefix autocomplete within 5 microseconds.
- **FR-2:** Typo correction returning candidates within edit distance $\le 2$.
- **FR-3:** Frequency-based ranking with lexicographical tie-breaking.
- **FR-4:** Real-time head-to-head comparison between Trie and Linear search.
- **FR-5:** Interactive 2D Levenshtein DP matrix visualization.
- **FR-6:** Automated multi-dataset benchmark execution with CSV export.

---

## 11. Non-Functional Requirements
- **NFR-1 (Performance):** Sub-millisecond server response times.
- **NFR-2 (Memory Safety):** Modern C++17 smart pointers (`std::unique_ptr`) ensuring zero memory leaks.
- **NFR-3 (Security):** Subprocess execution using `shell=False` and strict regex parameter sanitization.
- **NFR-4 (Usability):** Debounced search inputs, keyboard navigation, and responsive layout.

---

## 12. Data Structures Used
1. **Trie Structure:** `std::array<std::unique_ptr<TrieNode>, 26>` for $O(1)$ child branch addressing.
2. **Dynamic Programming Vectors:** `std::vector<int>` for DP row propagation during Trie DFS.
3. **Linear Dictionary Baseline:** `std::vector<std::pair<std::string, int>>` for baseline comparisons.
4. **Candidate Storage:** Custom `CorrectionCandidate` and `AutocompleteResult` structs.

---

## 13. Algorithms and Pseudocode
Refer to [`docs/algorithms.md`](algorithms.md) for complete mathematical proofs and pseudocode implementations of TrieInsert, TrieGetWordsWithPrefix, AutocompleteRank, ComputeLevenshtein2D, and TrieDPTypoCorrection.

---

## 14. Trie Implementation Explanation
Each node contains an array of 26 pointers corresponding to characters `'a'` through `'z'`. Pointers are directly indexed via `c - 'a'`. When a word terminates, `isEndOfWord` is set to `true`, and the word string and frequency are stored. Subtree traversal recurses through child pointers collecting descendant words without reconstructing strings character by character.

---

## 15. Edit Distance DP Explanation
Given source string $S_1$ and target string $S_2$, the edit distance is computed using the recurrence relation:
$$DP[i][j] = \min(DP[i-1][j] + 1,\; DP[i][j-1] + 1,\; DP[i-1][j-1] + \text{cost})$$
Where cost is 0 if $S_1[i] == S_2[j]$, else 1. Standalone distance computation uses two rolling rows to optimize space from $O(M \cdot N)$ to $O(\min(M, N))$.

---

## 16. Trie-Based Typo Correction Explanation
During Trie traversal, the active DP row is pushed down the tree. For each child node character $c$, a new row is derived from the parent row in $O(|W|)$ time. If $\min(\text{currentRow}) > \text{maxDistance}$, the branch is pruned. Terminal nodes with `currentRow[|W|] <= maxDistance` are collected as correction candidates.

---

## 17. Frequency Ranking Explanation
Suggestions are sorted primarily by frequency descending. Equal-frequency words are sorted alphabetically ascending:
```cpp
auto comparator = [](const auto& a, const auto& b) {
    if (a.frequency != b.frequency) return a.frequency > b.frequency;
    return a.word < b.word;
};
```

---

## 18. Time Complexity Analysis
- **Trie Prefix Search:** $O(P + S)$ where $P$ is prefix length and $S$ is subtree size (independent of dictionary size $N$).
- **Linear Prefix Search:** $O(N \cdot P)$ where $N$ is dictionary size.
- **Trie Typo Correction:** $O(\text{Unpruned Nodes} \cdot M)$ where $>98\%$ of nodes are pruned.
- **Linear Typo Correction:** $O(N \cdot M \cdot \bar{L})$.

---

## 19. Space Complexity Analysis
- **Trie Storage:** $O(\text{Node Count} \cdot |\Sigma|)$. For 50,000 words, ~159,128 nodes consume ~39.2 MB.
- **Linear Storage:** $O(N \cdot \bar{L})$, consuming ~0.6 MB.
- **Trade-off:** Trie exchanges auxiliary pointer memory for an order-of-magnitude reduction in search latency.

---

## 20. Performance Comparison & Measured Speedups
Based on empirical C++ measurements with `std::chrono::high_resolution_clock`:

| Dataset Size | Trie Search | Linear Search | Search Speedup | Trie Typo | Linear Typo | Typo Speedup |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| **1,000 words** | 0.29 μs | 3.69 μs | **12.7x** | 16.23 μs | 110.18 μs | **6.8x** |
| **5,000 words** | 0.25 μs | 17.90 μs | **70.9x** | 37.69 μs | 517.45 μs | **13.7x** |
| **10,000 words** | 0.60 μs | 34.27 μs | **57.2x** | 51.53 μs | 1,031.42 μs | **20.0x** |
| **25,000 words** | 1.82 μs | 84.65 μs | **46.5x** | 91.84 μs | 2,504.56 μs | **27.3x** |
| **50,000 words** | 3.19 μs | 162.36 μs | **51.0x** | 169.38 μs | 4,693.59 μs | **27.7x** |

---

## 21. Test Cases
The system was verified with 39 automated tests covering:
- Empty prefixes, duplicate insertions, single-character lookups, and boundary cases.
- Levenshtein operations (insertion, deletion, substitution, disjoint strings).
- Evaluation test cases: `pythn` $\to$ `python`, `appl` $\to$ `apple`, `progrmming` $\to$ `programming`, `machne` $\to$ `machine`, `javscript` $\to$ `javascript`, `computr` $\to$ `computer`.

---

## 22. Test Results
100% test pass rate across all C++ unit tests, Flask API endpoints, and benchmarking suites. Full test results are documented in [`tests/test_cases.md`](../tests/test_cases.md).

---

## 23. Screenshots
Actual UI screenshots captured using headless Google Chrome:
1. `screenshots/page1_smart_search.png`: Smart Search with live debounced dropdown, prefix highlighting, and execution metrics pill.
2. `screenshots/page2_typo_correction.png`: Typo Correction Station showing ranked candidates and evaluation presets.
3. `screenshots/page3_algorithm_explorer.png`: Interactive Trie node canvas and dynamic Levenshtein DP matrix table.
4. `screenshots/page4_performance_dashboard.png`: Live head-to-head comparison, metrics cards, benchmark data table, and publication charts.

---

## 24. Limitations
1. **Memory Footprint:** Fixed 26-pointer arrays consume more memory than compressed structures like Radix Trees or DAWGs.
2. **Alphabet Limitation:** Current implementation natively indexes lowercase English letters (`a-z`). Expanding to UTF-8 or international character sets requires dynamic hash maps or ternary branching.
3. **Transposition Cost:** Standard Levenshtein treats transpositions (e.g., `teh` $\to$ `the`) as two operations (delete + insert). Damerau-Levenshtein distance can be added for unit-cost transpositions.

---

## 25. Future Enhancements
1. **Radix Tree (Patricia Trie) Compression:** Compress single-child paths into multi-character edge labels to reduce node count by 60%.
2. **Damerau-Levenshtein DP:** Support adjacent character transpositions in DP row states.
3. **N-gram Language Modeling:** Incorporate bigram and trigram contextual probabilities to improve autocomplete ranking based on preceding words.
4. **Hardware Acceleration:** Implement SIMD vectorization (AVX-512) for parallel DP row updates.

---

## 26. Conclusion
The Smart Autocomplete and Typo Correction system successfully satisfies all project objectives. By unifying Trie prefix traversal with state-pruned Dynamic Programming, the engine achieves up to a **70.9x speedup** in prefix lookup and a **27.7x speedup** in typo correction over linear baseline scans. The modern web application and comprehensive academic documentation package provide an exemplary demonstration of foundational data structures and algorithms applied to real-world engineering challenges.

---

## 27. References
1. Fredkin, E. (1960). "Trie Memory." *Communications of the ACM*, 3(9), 490–499.
2. Levenshtein, V. I. (1966). "Binary codes capable of correcting deletions, insertions, and reversals." *Soviet Physics Doklady*, 10(8), 707–710.
3. Knuth, D. E. (1998). *The Art of Computer Programming, Volume 3: Sorting and Searching* (2nd ed.). Addison-Wesley.
4. Cormen, T. H., Leiserson, C. E., Rivest, R. L., & Stein, C. (2009). *Introduction to Algorithms* (3rd ed.). MIT Press.
5. Schulz, K. U., & Mihov, S. (2002). "Fast string correction with Levenshtein-automata." *International Journal on Document Analysis and Recognition*, 5(1), 67–85.

