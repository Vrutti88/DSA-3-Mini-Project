/**
 * @file test_autocomplete.cpp
 * @brief Unit tests for Autocomplete with frequency and alphabetical ranking.
 */

#include "Trie.h"
#include "Autocomplete.h"
#include <iostream>
#include <cassert>

void runAutocompleteTests() {
    std::cout << "--- [TEST SUITE: Autocomplete Engine] ---\n";

    dsa::Trie trie;
    // Insert words matching the prompt specification:
    // program = 900, progress = 700, programming = 500, programmer = 300
    trie.insert("program", 900);
    trie.insert("progress", 700);
    trie.insert("programming", 500);
    trie.insert("programmer", 300);
    trie.insert("programmable", 150);
    trie.insert("pro", 400);

    // Words with equal frequency for tie-break testing
    trie.insert("cat", 50);
    trie.insert("car", 50);
    trie.insert("cap", 50);
    trie.insert("cab", 50);

    dsa::Autocomplete ac(trie);
    double elapsedUs = 0.0;

    // Test 1: Empty input returns empty
    auto resEmpty = ac.getSuggestions("", 5, &elapsedUs);
    assert(resEmpty.empty());
    std::cout << "[PASS] Test 1: Empty prefix returns 0 results (" << elapsedUs << " us)\n";

    // Test 2: Frequency ranking for "pro"
    // Expect: program(900) -> progress(700) -> programming(500) -> pro(400) -> programmer(300)
    auto resPro = ac.getSuggestions("pro", 5, &elapsedUs);
    assert(resPro.size() == 5);
    assert(resPro[0].word == "program" && resPro[0].frequency == 900);
    assert(resPro[1].word == "progress" && resPro[1].frequency == 700);
    assert(resPro[2].word == "programming" && resPro[2].frequency == 500);
    assert(resPro[3].word == "pro" && resPro[3].frequency == 400);
    assert(resPro[4].word == "programmer" && resPro[4].frequency == 300);
    std::cout << "[PASS] Test 2: Frequency ranking for prefix 'pro' verified (" << elapsedUs << " us)\n";

    // Test 3: Top 3 limit check
    auto resProTop3 = ac.getSuggestions("pro", 3);
    assert(resProTop3.size() == 3);
    assert(resProTop3[0].word == "program");
    assert(resProTop3[1].word == "progress");
    assert(resProTop3[2].word == "programming");
    std::cout << "[PASS] Test 3: Top 3 limit verified\n";

    // Test 4: Equal-frequency tie-breaking (alphabetical order)
    // For prefix "ca", words have freq 50: cab, cap, car, cat
    auto resCa = ac.getSuggestions("ca", 5);
    assert(resCa.size() == 4);
    assert(resCa[0].word == "cab");
    assert(resCa[1].word == "cap");
    assert(resCa[2].word == "car");
    assert(resCa[3].word == "cat");
    std::cout << "[PASS] Test 4: Equal frequency alphabetical ordering verified\n";

    // Test 5: Prefix with no matches
    auto resNone = ac.getSuggestions("xyz", 5);
    assert(resNone.empty());
    std::cout << "[PASS] Test 5: Non-matching prefix returns empty\n";

    // Test 6: Case insensitivity and whitespace handling
    auto resCase = ac.getSuggestions("  PROGr  ", 5);
    assert(!resCase.empty());
    assert(resCase[0].word == "program");
    std::cout << "[PASS] Test 6: Case insensitivity & whitespace trimming verified\n";

    std::cout << ">>> ALL AUTOCOMPLETE TESTS PASSED SUCCESSFULLY! <<<\n\n";
}

int main() {
    runAutocompleteTests();
    return 0;
}
