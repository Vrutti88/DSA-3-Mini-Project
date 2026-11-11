/**
 * @file test_typo_correction.cpp
 * @brief Unit tests for Trie-based DP Typo Correction with branch pruning.
 */

#include "Trie.h"
#include "EditDistance.h"
#include <iostream>
#include <cassert>

void runTypoCorrectionTests() {
    std::cout << "--- [TEST SUITE: Trie-Based DP Typo Correction] ---\n";

    dsa::Trie trie;
    trie.insert("python", 950);
    trie.insert("apple", 870);
    trie.insert("application", 860);
    trie.insert("programming", 500);
    trie.insert("program", 900);
    trie.insert("programmer", 300);
    trie.insert("machine", 850);
    trie.insert("javascript", 920);
    trie.insert("computer", 880);
    trie.insert("computing", 650);

    double elapsedUs = 0.0;

    // Test 1: "pythn" -> expected "python" with edit distance 1
    auto res1 = dsa::EditDistance::correctWord(trie, "pythn", 2, 5, &elapsedUs);
    assert(!res1.empty());
    assert(res1[0].word == "python");
    assert(res1[0].editDistance == 1);
    std::cout << "[PASS] Test 1: 'pythn' -> 'python' (dist=" << res1[0].editDistance << ", time=" << elapsedUs << " us)\n";

    // Test 2: "appl" -> expected "apple" with edit distance 1
    auto res2 = dsa::EditDistance::correctWord(trie, "appl", 2, 5, &elapsedUs);
    assert(!res2.empty());
    assert(res2[0].word == "apple");
    assert(res2[0].editDistance == 1);
    std::cout << "[PASS] Test 2: 'appl' -> 'apple' (dist=" << res2[0].editDistance << ", time=" << elapsedUs << " us)\n";

    // Test 3: "progrmming" -> expected "programming" with edit distance 1
    auto res3 = dsa::EditDistance::correctWord(trie, "progrmming", 2, 5, &elapsedUs);
    assert(!res3.empty());
    assert(res3[0].word == "programming");
    assert(res3[0].editDistance == 1);
    std::cout << "[PASS] Test 3: 'progrmming' -> 'programming' (dist=" << res3[0].editDistance << ", time=" << elapsedUs << " us)\n";

    // Test 4: "machne" -> expected "machine" with edit distance 1
    auto res4 = dsa::EditDistance::correctWord(trie, "machne", 2, 5, &elapsedUs);
    assert(!res4.empty());
    assert(res4[0].word == "machine");
    assert(res4[0].editDistance == 1);
    std::cout << "[PASS] Test 4: 'machne' -> 'machine' (dist=" << res4[0].editDistance << ", time=" << elapsedUs << " us)\n";

    // Test 5: "javscript" -> expected "javascript" with edit distance 1
    auto res5 = dsa::EditDistance::correctWord(trie, "javscript", 2, 5, &elapsedUs);
    assert(!res5.empty());
    assert(res5[0].word == "javascript");
    assert(res5[0].editDistance == 1);
    std::cout << "[PASS] Test 5: 'javscript' -> 'javascript' (dist=" << res5[0].editDistance << ", time=" << elapsedUs << " us)\n";

    // Test 6: "computr" -> expected "computer" with edit distance 1
    auto res6 = dsa::EditDistance::correctWord(trie, "computr", 2, 5, &elapsedUs);
    assert(!res6.empty());
    assert(res6[0].word == "computer");
    assert(res6[0].editDistance == 1);
    std::cout << "[PASS] Test 6: 'computr' -> 'computer' (dist=" << res6[0].editDistance << ", time=" << elapsedUs << " us)\n";

    // Test 7: Ranking check (distance first, frequency second)
    // Suppose we query "progrm":
    // "program" has distance 1 (freq 900)
    // "programmer" has distance 4 (pruned if maxDistance=2)
    auto resProg = dsa::EditDistance::correctWord(trie, "progrm", 2, 5, &elapsedUs);
    assert(!resProg.empty());
    assert(resProg[0].word == "program");
    assert(resProg[0].editDistance == 1);
    std::cout << "[PASS] Test 7: 'progrm' -> 'program' ranked top\n";

    // Test 8: Unrelated word outside max distance
    auto resUnk = dsa::EditDistance::correctWord(trie, "zzzzzzzzz", 2, 5, &elapsedUs);
    assert(resUnk.empty());
    std::cout << "[PASS] Test 8: Distant unknown word safely pruned to 0 results\n";

    // Test 9: Boundary distance maxDistance=1 vs maxDistance=2
    auto resBound1 = dsa::EditDistance::correctWord(trie, "pyth", 1, 5, &elapsedUs);
    bool hasPyAt1 = false;
    for (const auto& c : resBound1) {
        if (c.word == "python") hasPyAt1 = true;
    }
    assert(!hasPyAt1); // Distance is 2, so excluded at 1

    auto resBound2 = dsa::EditDistance::correctWord(trie, "pyth", 2, 5, &elapsedUs);
    bool hasPyAt2 = false;
    for (const auto& c : resBound2) {
        if (c.word == "python") hasPyAt2 = true;
    }
    assert(hasPyAt2); // Included at 2
    std::cout << "[PASS] Test 9: Boundary distance K=1 vs K=2 verified\n";

    std::cout << ">>> ALL TRIE-BASED TYPO CORRECTION TESTS PASSED! <<<\n\n";
}

int main() {
    runTypoCorrectionTests();
    return 0;
}

