# System Architecture & Technical Specifications

**Course:** Data Structures & Algorithms - 3 (Mini-Project)  
**System Name:** Smart Autocomplete and Typo Correction Engine  
**Implementation Standard:** C++17 Core Engine + Python Flask Gateway + Vanilla Web Frontend  

---

## 1. High-Level Architecture Overview

The system employs a decoupled, multi-tiered architecture designed for separation of concerns, extreme query performance, and memory efficiency. The C++17 engine is the single source of truth for all data structures and algorithmic operations.

```
+-------------------------------------------------------------------------+
|                         PRESENTATION TIER (UI)                          |
|  - Smart Search (Debounced input, prefix highlighting, arrow navigation)|
|  - Typo Correction Station (Candidate ranking, max-distance tuning)     |
|  - Algorithm Explorer (Trie node canvas, dynamic 2D DP matrix view)    |
|  - Performance Dashboard (Live head-to-head race, benchmark tables)    |
+------------------------------------+------------------------------------+
                                     | HTTP / JSON (REST API)
                                     v
+-------------------------------------------------------------------------+
|                     APPLICATION & GATEWAY TIER (Flask)                  |
|  - Request Sanitization (Regex alphanumeric filtering, length limiting) |
|  - IPC Process Management (Subprocess execution / Daemon mode)          |
|  - Benchmark & Metrics Serialization (JSON, CSV generation)            |
+------------------------------------+------------------------------------+
                                     | Direct Process Pipe / Arguments
                                     v
+-------------------------------------------------------------------------+
|                         CORE DSA ENGINE (C++17)                         |
|  +-----------------------------+     +-------------------------------+  |
|  |       Trie Data Structure   |     |    Levenshtein DP Engine      |  |
|  | - 26-Pointer Array Nodes    |     | - Classical 2D DP Matrix      |  |
|  | - Frequency Storage         |     | - Trie Traversal + DP Row     |  |
|  | - Subtree DFS Word Collector|     | - Branch Pruning (min > K)    |  |
|  +-----------------------------+     +-------------------------------+  |
|  +-----------------------------+     +-------------------------------+  |
|  |     Autocomplete Ranker     |     |  Baseline Dictionary Scanner  |  |
|  | - Descending Frequency Sort |     | - Linear O(N·L) Prefix Scan   |  |
|  | - Lexicographical Tie-break |     | - Brute-Force O(N·M·L) DP     |  |
|  +-----------------------------+     +-------------------------------+  |
|  +-------------------------------------------------------------------+  |
|  |           Empirical Benchmarking & Metrics Module                 |  |
|  | - std::chrono::high_resolution_clock microsecond profiling        |  |
|  +-------------------------------------------------------------------+  |
+-------------------------------------------------------------------------+
```

---

## 2. Component Breakdown

### A. Core DSA Engine (`cpp/`)
1. **`TrieNode` & `Trie` (`Trie.h`, `Trie.cpp`):**
   - Implements an $m$-ary prefix tree using `std::array<std::unique_ptr<TrieNode>, 26>` for direct $O(1)$ child branch addressing.
   - Eliminates hash collisions, pointer chasing overhead, and bucket resizing.
   - Terminal nodes store `word` (full string), `frequency` (corpus weight), and `isEndOfWord` (boolean flag).
2. **`Autocomplete` (`Autocomplete.h`, `Autocomplete.cpp`):**
   - Navigates to prefix node in $O(P)$ time.
   - Collects matching descendant words via recursive subtree DFS.
   - Ranks results using `std::partial_sort` or `std::sort` with custom multi-tier lambda comparator.
3. **`EditDistance` (`EditDistance.h`, `EditDistance.cpp`):**
   - `calculateDistance()`: Standalone space-optimized $O(\min(M, N))$ Levenshtein dynamic programming.
   - `getDPMatrix()`: Generates full $(M+1) \times (N+1)$ matrix for academic inspection.
   - `searchTrieDP()`: Traverses the Trie while updating DP row states. Prunes entire branches whenever $\min(\text{currentRow}) > \text{maxDistance}$.
4. **`DictionaryScanner` (`DictionaryScanner.h`, `DictionaryScanner.cpp`):**
   - Holds linear vector `std::vector<std::pair<std::string, int>>`.
   - Sequential scans for prefix matching and brute-force Levenshtein typo correction.
   - Ensures identical queries, dictionary data, and ranking criteria for unbiased benchmarking.
5. **`BenchmarkRunner` (`Benchmark.h`, `Benchmark.cpp`):**
   - Runs automated evaluations across 1k, 5k, 10k, 25k, and 50k word datasets.
   - Executes repeated trials to eliminate operating system scheduling noise.
   - Computes speedup multipliers and outputs structured CSV and JSON logs.

### B. Backend API Gateway (`backend/`)
- **`engine_client.py`:** Encapsulates C++ binary execution using `subprocess.run` with `shell=False` to prevent command injection. Sanitizes input strings using regular expressions.
- **`app.py`:** Exposes cleanly structured REST API endpoints and serves static frontend assets.

---

## 3. REST API Specification

| Endpoint | Method | Query Parameters | Description | Sample Output |
| :--- | :--- | :--- | :--- | :--- |
| `/api/health` | `GET` | None | Engine status & system sanity check | `{"status":"healthy","language":"C++17"}` |
| `/api/autocomplete` | `GET` | `q` (string), `limit` (int) | Fast Trie prefix autocomplete | `{"query":"pro","count":5,"results":[...]}` |
| `/api/correct` | `GET` | `q` (string), `max_dist` (int), `limit` (int) | Trie DP typo correction with pruning | `{"query":"pythn","results":[{"word":"python","edit_distance":1}]}` |
| `/api/search` | `GET` | `q` (string), `limit` (int) | Autocomplete with automatic typo fallback | `{"exact_match":false,"typo_corrections":[...]}` |
| `/api/compare` | `GET` | `q` (string), `limit` (int) | Real-time Head-to-Head Trie vs Linear Race | `{"trie_time_us":2.67,"linear_time_us":36.33,"speedup_factor":13.62}` |
| `/api/dp-matrix` | `GET` | `s1` (string), `s2` (string) | Full 2D Levenshtein table & distance | `{"distance":3,"matrix":[[0,1,...],...]}` |
| `/api/dictionary/stats` | `GET` | None | Dictionary word count, node count, memory | `{"word_count":10000,"trie_node_count":32887,"trie_memory_mb":8.098}` |
| `/api/benchmark` | `GET`, `POST` | `run` (bool) | Retrieve or execute full benchmark suite | `{"status":"success","results":[...]}` |

---

## 4. Non-Functional Requirements
1. **Low Latency:** Trie prefix search executes in $< 5 \text{ μs}$ on a 10,000-word dictionary.
2. **Security & Injection Prevention:** Subprocess parameters are passed as explicit token arrays (`shell=False`) after strict alphanumeric sanitization.
3. **Reproducibility:** Benchmarks are recorded directly via `std::chrono` on hardware without synthetic or simulated timing values.
4. **Modularity & Maintainability:** Zero global state leaks; separation into independent headers and compilation units.

