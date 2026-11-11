/**
 * @file test_trie.cpp
 * @brief Unit tests for Trie data structure.
 */

#include "Trie.h"
#include <iostream>
#include <cassert>
#include <vector>
#include <algorithm>

void runTrieTests() {
    std::cout << "--- [TEST SUITE: Trie Operations] ---\n";

    dsa::Trie trie;

    // Test 1: Empty Trie properties
    assert(trie.getWordCount() == 0);
    assert(trie.getNodeCount() == 1); // Root node exists
    assert(!trie.search("apple"));
    assert(!trie.startsWith("app"));
    std::cout << "[PASS] Test 1: Empty Trie verified\n";

    // Test 2: Insert words
    assert(trie.insert("apple", 100));
    assert(trie.insert("app", 50));
    assert(trie.insert("application", 200));
    assert(trie.insert("apply", 75));
    assert(trie.insert("banana", 80));
    assert(trie.getWordCount() == 5);
    std::cout << "[PASS] Test 2: Basic insertions verified\n";

    // Test 3: Duplicate insertion and frequency update
    bool isNew = trie.insert("apple", 250);
    assert(!isNew); // Already existed
    assert(trie.getWordCount() == 5); // Count unchanged
    const dsa::TrieNode* appleNode = trie.getNode("apple");
    assert(appleNode != nullptr);
    assert(appleNode->frequency == 250); // Updated frequency
    std::cout << "[PASS] Test 3: Duplicate handling and frequency update verified\n";

    // Test 4: Search existing and non-existing
    assert(trie.search("apple"));
    assert(trie.search("app"));
    assert(trie.search("application"));
    assert(trie.search("banana"));
    assert(!trie.search("appl")); // Prefix, but not a full word
    assert(!trie.search("orange"));
    assert(!trie.search(""));
    std::cout << "[PASS] Test 4: Search exact words verified\n";

    // Test 5: Prefix checking
    assert(trie.startsWith("app"));
    assert(trie.startsWith("appl"));
    assert(trie.startsWith("applic"));
    assert(trie.startsWith("ban"));
    assert(!trie.startsWith("cat"));
    assert(!trie.startsWith("applex"));
    std::cout << "[PASS] Test 5: Prefix checking verified\n";

    // Test 6: Prefix matching collection
    auto appMatches = trie.getWordsWithPrefix("app");
    // Should have: app (50), apple (250), application (200), apply (75) = 4 matches
    assert(appMatches.size() == 4);
    std::vector<std::string> words;
    for (const auto& item : appMatches) {
        words.push_back(item.first);
    }
    std::sort(words.begin(), words.end());
    assert(words[0] == "app");
    assert(words[1] == "apple");
    assert(words[2] == "application");
    assert(words[3] == "apply");
    std::cout << "[PASS] Test 6: Prefix word collection verified\n";

    // Test 7: Memory estimation and clear
    size_t mem = trie.estimateMemoryUsage();
    assert(mem > 0);
    trie.clear();
    assert(trie.getWordCount() == 0);
    assert(trie.getNodeCount() == 1);
    assert(!trie.search("apple"));
    std::cout << "[PASS] Test 7: Memory estimation and clear verified\n";

    std::cout << ">>> ALL TRIE TESTS PASSED SUCCESSFULLY! <<<\n\n";
}

int main() {
    runTrieTests();
    return 0;
}
