# Requirement Traceability Matrix (RTM)

**Project Title:** Smart Autocomplete and Typo Correction Engine  
**Course Code:** DSA-3 Mini Project Evaluation (100 Marks)  
**Evaluation Standard:** Strict Academic Compliance  

---

## Master Traceability Matrix

| Requirement ID | Requirement Description | Implementation Source File(s) | Verification Test File(s) | Status | Measured Evidence & Artifacts |
| :---: | :--- | :--- | :--- | :---: | :--- |
| **REQ-01** | C++17 Trie from scratch with 26-pointer array | `cpp/include/Trie.h`<br>`cpp/src/Trie.cpp` | `tests/cpp_tests/audit_phase3_trie.cpp`<br>`tests/cpp_tests/test_trie.cpp` | **PASS** | `std::array<std::unique_ptr<TrieNode>, 26>` allocates $O(1)$ child indexing; handles duplicates; zero memory leaks. |
| **REQ-02** | Prefix Search & Subtree DFS Word Retrieval | `cpp/include/Trie.h`<br>`cpp/src/Trie.cpp` | `tests/cpp_tests/audit_phase3_trie.cpp` | **PASS** | `getWordsWithPrefix("app")` retrieves `{"app", "apple", "application", "apply"}` in $1.2 \text{ μs}$. |
| **REQ-03** | Frequency-Based Autocomplete Ranking | `cpp/include/Autocomplete.h`<br>`cpp/src/Autocomplete.cpp` | `tests/cpp_tests/audit_phase4_autocomplete.cpp`<br>`tests/cpp_tests/test_autocomplete.cpp` | **PASS** | Tested on inputs `"pro"`, `"prog"`, `"app"`, `"py"`, `"jav"`, `"comp"`, `"xyz"`, `""`, `"p"`. Ranks descending frequency; top 5 default. |
| **REQ-04** | Alphabetical Tie-Breaking on Equal Frequency | `cpp/src/Autocomplete.cpp`<br>`cpp/src/EditDistance.cpp` | `tests/cpp_tests/audit_phase4_autocomplete.cpp` | **PASS** | Lexicographical order strictly enforced (`alpha < bravo < charlie < delta`) on equal frequency scores. |
| **REQ-05** | Standalone 2D Levenshtein Dynamic Programming | `cpp/include/EditDistance.h`<br>`cpp/src/EditDistance.cpp` | `tests/cpp_tests/audit_phase5_edit_distance.cpp`<br>`tests/cpp_tests/test_edit_distance.cpp` | **PASS** | Tested: `kitten->sitting=3`, `horse->ros=3`, `book->back=2`, `python->python=0`, `pythn->python=1`, `cat->cut=1`, `""->apple=5`. |
| **REQ-06** | Trie Traversal with DP Row State & Branch Pruning | `cpp/include/EditDistance.h`<br>`cpp/src/EditDistance.cpp` | `tests/cpp_tests/audit_phase6_typo.cpp`<br>`tests/cpp_tests/test_typo_correction.cpp` | **PASS** | DFS propagates DP rows; prunes when $\min(\text{row}) > K$. Tested: `pythn`, `appl`, `javscript`, `machne`, `progrmming`, `computr`. |
| **REQ-07** | Boundary & Unrelated Word Typo Pruning | `cpp/src/EditDistance.cpp` | `tests/cpp_tests/audit_phase6_typo.cpp` | **PASS** | Verified that `"pyth"` is excluded at $K=1$ and included at $K=2$; `"zzzzzzzzzz"` safely prunes to 0 results. |
| **REQ-08** | Linear Dictionary Scanning Baseline | `cpp/include/DictionaryScanner.h`<br>`cpp/src/DictionaryScanner.cpp` | `cpp/src/Benchmark.cpp` | **PASS** | Independent sequential scan with identical queries and ranking rules; does NOT use Trie internally. |
| **REQ-09** | Empirical Benchmarking Suite (1k to 50k words) | `cpp/include/Benchmark.h`<br>`cpp/src/Benchmark.cpp` | `bin/engine --benchmark`<br>`benchmarks/generate_graphs.py` | **PASS** | Measures build time, search time, typo time, memory across 1k, 5k, 10k, 25k, 50k words using `std::chrono` (50 trials). CSV/JSON exported. |
| **REQ-10** | Publication-Quality Performance Charts | `benchmarks/generate_graphs.py` | `benchmarks/graphs/*.png` | **PASS** | 3 high-res charts generated from authentic CSV data: search comparison, scaling vs dictionary size, and speedup factors. |
| **REQ-11** | Python Flask REST API Gateway | `backend/app.py`<br>`backend/engine_client.py` | `tests/api_tests/test_api.py` | **PASS** | 7 REST endpoints tested: `/api/health`, `/api/autocomplete`, `/api/correct`, `/api/search`, `/api/compare`, `/api/dp-matrix`, `/api/benchmark`. |
| **REQ-12** | Subprocess Security & Parameter Sanitization | `backend/engine_client.py` | `scripts/test_scenarios.py` | **PASS** | Invokes C++ binary with `shell=False` and token array; sanitizes input using regex `re.sub(r'[^a-zA-Z0-9]', '', query)`. |
| **REQ-13** | Page 1: Smart Search Interface | `frontend/index.html`<br>`frontend/js/script.js` | Browser Inspection / Screenshots | **PASS** | Rounded search box, 120ms debouncing, prefix highlighting, arrow key navigation, frequency badges, auto "Did You Mean" fallback. |
| **REQ-14** | Page 2: Typo Correction Station | `frontend/index.html`<br>`frontend/js/script.js` | Browser Inspection / Screenshots | **PASS** | Typo test input, distance badge, candidate ranking table, preset buttons (`pythn`, `appl`, `machne`, etc.), "Adopt Word" button. |
| **REQ-15** | Page 3: Algorithm Explorer & DP Visualizer | `frontend/index.html`<br>`frontend/js/script.js` | Browser Inspection / Screenshots | **PASS** | HTML5 Canvas Trie branch tree visualizer, interactive 2D Levenshtein table with color-coded cells, and pruning explanations. |
| **REQ-16** | Page 4: Performance Dashboard & Head-to-Head Race | `frontend/index.html`<br>`frontend/js/script.js` | Browser Inspection / Screenshots | **PASS** | Live Head-to-Head Trie vs Linear race, metrics cards (words, nodes, memory), 5-dataset benchmark table, and chart gallery. |
| **REQ-17** | End-to-End User Scenario Validation | Full-stack application | `scripts/test_scenarios.py` | **PASS** | All 4 mandatory end-to-end user scenarios executed against running application with 100% assertions satisfied. |
| **REQ-18** | Comprehensive Academic Documentation | `docs/*.md`<br>`README.md` | Document Audit | **PASS** | Master 27-section project report, RTM, defect log, test catalog, complexity proofs, and 45+ Viva Q&A guide. |

