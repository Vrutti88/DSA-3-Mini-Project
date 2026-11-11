# Defect Tracking & Bug Fix Log

**Project:** Smart Autocomplete and Typo Correction Engine  
**Course Code:** DSA-3 Mini Project Evaluation  
**Auditor:** Strict Senior QA & C++ Systems Architect  

This document logs all defects, compilation bottlenecks, and runtime issues discovered during development and auditing, detailing the root cause, exact source file and line, the implemented resolution, and regression testing verification.

---

## Defect Log Summary

| Bug ID | Component | Severity | Description | Status |
| :---: | :---: | :---: | :---: | :---: |
| **BUG-01** | `Trie.h` | High (Compilation Blocker) | `std::array::fill(nullptr)` copy-assignment deletion on `unique_ptr` | **RESOLVED** |
| **BUG-02** | `EditDistance.h` | High (Compilation Blocker) | Missing default constructor on `CorrectionCandidate` required by `std::vector::resize()` | **RESOLVED** |
| **BUG-03** | `EditDistance.cpp`| Medium (Compilation Error) | Redundant `const` qualifier on static method definition | **RESOLVED** |
| **BUG-04** | `generate_graphs.py` | Medium (Runtime Crash) | Matplotlib AppKit WindowServer access in headless macOS environment | **RESOLVED** |
| **BUG-05** | `app.py` | Medium (Import Failure) | `ModuleNotFoundError: No module named 'backend'` when executed without `PYTHONPATH` | **RESOLVED** |
| **BUG-06** | `script.js` | Low (UI Navigation) | Deep link URL hash navigation not activating tab panes on initial load | **RESOLVED** |

---

## Detailed Root Cause Analysis & Resolutions

### BUG-01: `std::array::fill` on Move-Only `std::unique_ptr`
- **Location:** `cpp/include/Trie.h`, line 44
- **Symptom:** Clang compilation error: `object of type 'std::unique_ptr<dsa::TrieNode>' cannot be assigned because its copy assignment operator is implicitly deleted`.
- **Root Cause:** In C++17, `std::array::fill(const T& value)` takes a const reference and invokes the copy assignment operator `*first = value` for each element. Because `std::unique_ptr` is a move-only type with deleted copy semantics, `fill()` fails at compile time.
- **Resolution:** Replaced `children.fill(nullptr)` with an explicit range-based loop over elements:
  ```cpp
  TrieNode() : isEndOfWord(false), frequency(0), word("") {
      for (auto& child : children) {
          child = nullptr;
      }
  }
  ```
- **Verification:** Recompiled with `clang++ -std=c++17 -Wall -Wextra -Wpedantic`. Compiled cleanly with 0 warnings.

---

### BUG-02: Missing Default Constructor for `std::vector::resize()`
- **Location:** `cpp/include/EditDistance.h`, line 25
- **Symptom:** Clang compilation error: `no matching constructor for initialization of 'dsa::CorrectionCandidate' in std::vector::resize`.
- **Root Cause:** When `std::vector::resize(size_t n)` is invoked on a vector where `n` could exceed current capacity, the standard template library requires `value_type` to satisfy the `DefaultConstructible` concept.
- **Resolution:** Added an explicit default constructor to `CorrectionCandidate`:
  ```cpp
  struct CorrectionCandidate {
      std::string word;
      int editDistance;
      int frequency;

      CorrectionCandidate() : word(""), editDistance(0), frequency(0) {}
      CorrectionCandidate(std::string w, int dist, int freq)
          : word(std::move(w)), editDistance(dist), frequency(freq) {}
  };
  ```
- **Verification:** `test_typo_correction.cpp` compiled and executed with 100% pass rate.

---

### BUG-03: Redundant `const` on Static Method Definition
- **Location:** `cpp/src/EditDistance.cpp`, line 118
- **Symptom:** Compiler error: `static member function cannot have 'const' qualifier`.
- **Root Cause:** In C++, member functions declared `static` do not receive an implicit `this` pointer argument; therefore, specifying `const` on the method definition is a syntax violation.
- **Resolution:** Removed the trailing `const` qualifier from `EditDistance::correctWord` in `EditDistance.cpp`.
- **Verification:** Clean compilation with `-Wall -Wextra -Wpedantic`.

---

### BUG-04: Matplotlib WindowServer Connection in Headless Environment
- **Location:** `benchmarks/generate_graphs.py`, line 8
- **Symptom:** Python process exit code 134 with CoreGraphics DisplayLink errors: `CVDisplayLinkCreateWithCGDisplay failed`.
- **Root Cause:** By default on macOS, Matplotlib selects the interactive GUI backend (`MacOSX`), which attempts to connect to the WindowServer via Cocoa/AppKit. In headless or sandboxed automated environments, GUI connections are disallowed.
- **Resolution:** Forced Matplotlib to use the pure headless raster backend (`Agg`) before importing `pyplot`:
  ```python
  import matplotlib
  matplotlib.use('Agg')
  import matplotlib.pyplot as plt
  ```
- **Verification:** `generate_graphs.py` executed cleanly, writing all 3 high-resolution chart files in under 1.2 seconds.

---

### BUG-05: Missing Root Directory in Python `sys.path`
- **Location:** `backend/app.py`, line 10
- **Symptom:** `ModuleNotFoundError: No module named 'backend'` when executing `python3 backend/app.py` from outside workspace root or without setting `PYTHONPATH=.`.
- **Root Cause:** Python executes scripts with `sys.path[0]` set to the script's directory (`backend/`), causing top-level imports like `from backend.engine_client import EngineClient` to fail.
- **Resolution:** Injected project root dynamically into `sys.path`:
  ```python
  BASE_DIR = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
  if BASE_DIR not in sys.path:
      sys.path.insert(0, BASE_DIR)
  ```
- **Verification:** Tested running `app.py` from root, `backend/`, and external working directories. All imports succeed reliably.

---

### BUG-06: Deep Link URL Hash Navigation
- **Location:** `frontend/js/script.js`, line 45
- **Symptom:** Navigating directly to `/#tab-typo` or `/#tab-dashboard` left the Smart Search tab visible instead of switching to the requested pane.
- **Root Cause:** Tab activation logic was attached solely to click events on navbar buttons, without inspecting `window.location.hash` during `DOMContentLoaded`.
- **Resolution:** Added hash routing parsing and direct pane activation:
  ```javascript
  const hash = window.location.hash.replace("#", "");
  if (hash && document.getElementById(hash)) {
      activateTab(hash);
  }
  ```
- **Verification:** Verified that direct URLs `http://127.0.0.1:5001/#tab-typo`, `/#tab-explorer`, and `/#tab-dashboard` open their respective views immediately.

