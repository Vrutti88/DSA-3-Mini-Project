/**
 * @file DictionaryScanner.h
 * @brief Baseline Linear Dictionary Scanner for algorithmic benchmarking.
 * 
 * Part of DSA-3 Mini Project: Smart Autocomplete and Typo Correction.
 */

#ifndef DICTIONARY_SCANNER_H
#define DICTIONARY_SCANNER_H

#include <string>
#include <vector>
#include <utility>
#include "EditDistance.h"

namespace dsa {

/**
 * @brief Baseline class that scans an array of words linearly.
 * Used exclusively for rigorous academic performance comparison against Trie.
 */
class DictionaryScanner {
private:
    std::vector<std::pair<std::string, int>> dictionary;

public:
    DictionaryScanner() = default;

    /**
     * @brief Loads word-frequency pairs from a file into linear storage.
     */
    bool loadFromFile(const std::string& filepath);

    /**
     * @brief Adds a word with frequency directly.
     */
    void addWord(const std::string& word, int freq);

    /**
     * @brief Performs prefix search by sequentially scanning all N dictionary words.
     * Time Complexity: O(N * L + M log K), where N is dictionary size, L is word length.
     * 
     * @param prefix Target prefix.
     * @param limit Top suggestions to return.
     * @param outExecutionTimeUs Optional output for runtime in microseconds.
     */
    std::vector<std::pair<std::string, int>> searchPrefix(
        const std::string& prefix,
        size_t limit = 5,
        double* outExecutionTimeUs = nullptr
    ) const;

    /**
     * @brief Baseline typo correction by computing edit distance against ALL N words.
     * Time Complexity: O(N * L1 * L2).
     */
    std::vector<CorrectionCandidate> correctWord(
        const std::string& query,
        int maxDistance = 2,
        size_t limit = 5,
        double* outExecutionTimeUs = nullptr
    ) const;

    size_t size() const { return dictionary.size(); }
    void clear() { dictionary.clear(); }
    const std::vector<std::pair<std::string, int>>& getEntries() const { return dictionary; }
};

} // namespace dsa

#endif // DICTIONARY_SCANNER_H

