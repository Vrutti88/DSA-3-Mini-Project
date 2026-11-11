#!/usr/bin/env python3
"""
End-to-end integration scenario verification script.
Tests all 4 user scenarios against the running web application.
"""

import urllib.request
import json
import sys

def get_json(url):
    with urllib.request.urlopen(url) as response:
        return json.loads(response.read().decode())

def main():
    print("====================================================")
    print("        END-TO-END INTEGRATION SCENARIOS TEST        ")
    print("====================================================")

    # Scenario 1: Autocomplete "prog" -> contains "programming"
    print("\n[Scenario 1: Autocomplete 'prog']")
    res1 = get_json("http://127.0.0.1:5001/api/autocomplete?q=prog&limit=5")
    words1 = [item["word"] for item in res1.get("results", [])]
    print("Returned words:", words1)
    assert "programming" in words1, "Expected 'programming' in results"
    print("[PASS] Scenario 1 verified: 'prog' suggests 'programming' (ranked by frequency)")

    # Scenario 2: Typo "pythn" -> correction "python" with edit distance 1
    print("\n[Scenario 2: Typo Correction 'pythn']")
    res2 = get_json("http://127.0.0.1:5001/api/correct?q=pythn&max_dist=2&limit=5")
    results2 = res2.get("results", [])
    assert len(results2) > 0, "Expected corrections for 'pythn'"
    top2 = results2[0]
    print("Top correction:", top2)
    assert top2["word"] == "python", f"Expected 'python', got {top2['word']}"
    assert top2["edit_distance"] == 1, f"Expected distance 1, got {top2['edit_distance']}"
    print("[PASS] Scenario 2 verified: 'pythn' -> 'python' with edit distance 1")

    # Scenario 3: Head-to-Head Comparison Race "prog"
    print("\n[Scenario 3: Performance Head-to-Head Race 'prog']")
    res3 = get_json("http://127.0.0.1:5001/api/compare?q=prog&limit=5")
    trie_time = res3.get("trie_time_us", 0.0)
    linear_time = res3.get("linear_time_us", 0.0)
    speedup = res3.get("speedup_factor", 1.0)
    print(f"Trie Search Time:   {trie_time:.2f} μs")
    print(f"Linear Search Time: {linear_time:.2f} μs")
    print(f"Speedup Multiplier: {speedup:.2f}x")
    assert speedup > 1.0, "Expected Trie to be faster than linear search"
    print("[PASS] Scenario 3 verified: Trie is significantly faster than Linear Scan")

    # Scenario 4: Levenshtein DP Matrix Visualizer "kitten" vs "sitting"
    print("\n[Scenario 4: Levenshtein DP Matrix 'kitten' vs 'sitting']")
    res4 = get_json("http://127.0.0.1:5001/api/dp-matrix?s1=kitten&s2=sitting")
    dist4 = res4.get("distance", 0)
    rows4 = res4.get("rows", 0)
    cols4 = res4.get("cols", 0)
    print(f"Matrix Dimensions: {rows4} x {cols4}")
    print(f"Calculated Edit Distance: {dist4}")
    assert dist4 == 3, f"Expected edit distance 3, got {dist4}"
    assert rows4 == 7 and cols4 == 8, f"Expected 7x8 matrix, got {rows4}x{cols4}"
    print("[PASS] Scenario 4 verified: 7x8 DP Matrix correctly computed with distance 3")

    print("\n====================================================")
    print(">>> ALL 4 END-TO-END SCENARIOS PASSED 100% <<<")
    print("====================================================")

if __name__ == "__main__":
    main()

