/**
 * @file EditDistance.h
 * @brief Dynamic Programming Edit Distance & Trie-based Typo Correction Engine.
 * 
 * Part of DSA-3 Mini Project: Smart Autocomplete and Typo Correction.
 */

#ifndef EDIT_DISTANCE_H
#define EDIT_DISTANCE_H

#include "Trie.h"
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>

namespace dsa {

/**
 * @brief Structure representing a typo correction candidate.
 */
struct CorrectionCandidate {
    std::string word;
    int editDistance;
    int frequency;

    CorrectionCandidate() : word(""), editDistance(0), frequency(0) {}

    CorrectionCandidate(std::string w, int dist, int freq)
        : word(std::move(w)), editDistance(dist), frequency(freq) {}
};

/**
 * @brief Class encapsulating Levenshtein DP and Trie-traversal typo correction.
 */
class EditDistance {
private:
    /**
     * @brief Recursive DFS helper traversing the Trie while computing DP rows.
     * Prunes branches where min(dpRow) > maxDistance.
     */
    static void searchTrieDP(
        const TrieNode* node,
        char lastChar,
        const std::string& target,
        int maxDistance,
        const std::vector<int>& previousRow,
        std::vector<CorrectionCandidate>& candidates
    );

public:
    /**
     * @brief Computes classic 2D Levenshtein distance between two strings using DP.
     * Allowed operations: Insertion (+1), Deletion (+1), Substitution (+1).
     * Time Complexity: O(M * N), Space Complexity: O(M * N) or O(min(M, N)).
     * 
     * @param s1 First string.
     * @param s2 Second string.
     * @return Minimum edit distance.
     */
    static int calculateDistance(const std::string& s1, const std::string& s2);

    /**
     * @brief Generates full 2D DP matrix for educational visualization and analysis.
     * @param s1 Source string (row headers).
     * @param s2 Target string (column headers).
     * @return 2D vector of size (s1.length() + 1) x (s2.length() + 1).
     */
    static std::vector<std::vector<int>> getDPMatrix(const std::string& s1, const std::string& s2);

    /**
     * @brief Performs typo correction using DP row state propagation across Trie branches.
     * Only explores paths with min(row) <= maxDistance.
     * 
     * @param trie Reference to the dictionary Trie.
     * @param query Misspelled query string.
     * @param maxDistance Maximum allowed edit distance (default 2).
     * @param limit Maximum number of candidate suggestions to return (default 5).
     * @param outExecutionTimeUs Optional output parameter for runtime in microseconds.
     * @return Ranked list of correction candidates.
     */
    static std::vector<CorrectionCandidate> correctWord(
        const Trie& trie,
        const std::string& query,
        int maxDistance = 2,
        size_t limit = 5,
        double* outExecutionTimeUs = nullptr
    );
};

} // namespace dsa

#endif // EDIT_DISTANCE_H
