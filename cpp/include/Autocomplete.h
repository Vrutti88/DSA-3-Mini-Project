/**
 * @file Autocomplete.h
 * @brief Autocomplete suggestion generator with frequency-based ranking.
 * 
 * Part of DSA-3 Mini Project: Smart Autocomplete and Typo Correction.
 */

#ifndef AUTOCOMPLETE_H
#define AUTOCOMPLETE_H

#include "Trie.h"
#include <string>
#include <vector>
#include <chrono>

namespace dsa {

/**
 * @brief Represents a single autocomplete recommendation.
 */
struct AutocompleteResult {
    std::string word;
    int frequency;

    AutocompleteResult(std::string w, int f) : word(std::move(w)), frequency(f) {}
};

/**
 * @brief Autocomplete engine using Trie prefix traversal and frequency ranking.
 */
class Autocomplete {
private:
    const Trie& trie;

public:
    explicit Autocomplete(const Trie& trieRef) : trie(trieRef) {}

    /**
     * @brief Generates top suggestions for a given prefix.
     * 
     * Algorithm:
     * 1. Traverse to prefix node in the Trie: O(P) where P = prefix length.
     * 2. Traverse the subtree rooted at prefix node to collect candidate words: O(S) where S is size of subtree.
     * 3. Sort candidates: primary by frequency (descending), secondary by alphabetical order (ascending).
     * 4. Return top `limit` suggestions (default: 5).
     * 
     * @param prefix Input prefix query string.
     * @param limit Maximum number of suggestions to return (default 5).
     * @param outExecutionTimeUs Optional output parameter to record elapsed time in microseconds.
     * @return Vector of AutocompleteResult ordered by rank.
     */
    std::vector<AutocompleteResult> getSuggestions(
        const std::string& prefix, 
        size_t limit = 5,
        double* outExecutionTimeUs = nullptr
    ) const;
};

} // namespace dsa

#endif // AUTOCOMPLETE_H
