/**
 * @file EditDistance.cpp
 * @brief Implementation of 2D DP Levenshtein and Trie-based DP Typo Correction.
 */

#include "EditDistance.h"
#include <algorithm>
#include <cmath>
#include <cctype>

namespace dsa {

int EditDistance::calculateDistance(const std::string& s1, const std::string& s2) {
    size_t m = s1.size();
    size_t n = s2.size();

    // dp[i][j] stores edit distance between s1[0..i-1] and s2[0..j-1]
    std::vector<int> prev(n + 1);
    std::vector<int> curr(n + 1);

    for (size_t j = 0; j <= n; ++j) {
        prev[j] = static_cast<int>(j);
    }

    for (size_t i = 1; i <= m; ++i) {
        curr[0] = static_cast<int>(i);
        char c1 = static_cast<char>(std::tolower(static_cast<unsigned char>(s1[i - 1])));

        for (size_t j = 1; j <= n; ++j) {
            char c2 = static_cast<char>(std::tolower(static_cast<unsigned char>(s2[j - 1])));
            if (c1 == c2) {
                curr[j] = prev[j - 1]; // No operation needed
            } else {
                int insertOp = curr[j - 1] + 1;
                int deleteOp = prev[j] + 1;
                int replaceOp = prev[j - 1] + 1;
                curr[j] = std::min({insertOp, deleteOp, replaceOp});
            }
        }
        prev = curr;
    }

    return prev[n];
}

std::vector<std::vector<int>> EditDistance::getDPMatrix(const std::string& s1, const std::string& s2) {
    size_t m = s1.size();
    size_t n = s2.size();

    std::vector<std::vector<int>> matrix(m + 1, std::vector<int>(n + 1, 0));

    for (size_t i = 0; i <= m; ++i) {
        matrix[i][0] = static_cast<int>(i);
    }
    for (size_t j = 0; j <= n; ++j) {
        matrix[0][j] = static_cast<int>(j);
    }

    for (size_t i = 1; i <= m; ++i) {
        char c1 = static_cast<char>(std::tolower(static_cast<unsigned char>(s1[i - 1])));
        for (size_t j = 1; j <= n; ++j) {
            char c2 = static_cast<char>(std::tolower(static_cast<unsigned char>(s2[j - 1])));
            if (c1 == c2) {
                matrix[i][j] = matrix[i - 1][j - 1];
            } else {
                int insertOp = matrix[i][j - 1] + 1;
                int deleteOp = matrix[i - 1][j] + 1;
                int replaceOp = matrix[i - 1][j - 1] + 1;
                matrix[i][j] = std::min({insertOp, deleteOp, replaceOp});
            }
        }
    }

    return matrix;
}

void EditDistance::searchTrieDP(
    const TrieNode* node,
    char lastChar,
    const std::string& target,
    int maxDistance,
    const std::vector<int>& previousRow,
    std::vector<CorrectionCandidate>& candidates
) {
    size_t targetLen = target.size();
    std::vector<int> currentRow(targetLen + 1);

    // Cost of deleting character lastChar from the trie prefix
    currentRow[0] = previousRow[0] + 1;

    // Fill DP row for this Trie character against target word
    for (size_t j = 1; j <= targetLen; ++j) {
        int insertCost = currentRow[j - 1] + 1;
        int deleteCost = previousRow[j] + 1;
        int replaceCost = previousRow[j - 1] + (lastChar == target[j - 1] ? 0 : 1);

        currentRow[j] = std::min({insertCost, deleteCost, replaceCost});
    }

    // Check if current node is a word and distance to target is within maxDistance
    if (node->isEndOfWord && currentRow[targetLen] <= maxDistance) {
        candidates.emplace_back(node->word, currentRow[targetLen], node->frequency);
    }

    // Branch Pruning:
    // If the minimum element in currentRow > maxDistance, no descendant can ever achieve <= maxDistance.
    int minRowVal = *std::min_element(currentRow.begin(), currentRow.end());
    if (minRowVal <= maxDistance) {
        for (int i = 0; i < 26; ++i) {
            if (node->children[i]) {
                char nextChar = static_cast<char>('a' + i);
                searchTrieDP(node->children[i].get(), nextChar, target, maxDistance, currentRow, candidates);
            }
        }
    }
}

std::vector<CorrectionCandidate> EditDistance::correctWord(
    const Trie& trie,
    const std::string& query,
    int maxDistance,
    size_t limit,
    double* outExecutionTimeUs
) {
    auto startTime = std::chrono::high_resolution_clock::now();

    // Sanitize query to lowercase alphabetic
    std::string target = "";
    target.reserve(query.size());
    for (char c : query) {
        if (!std::isspace(static_cast<unsigned char>(c))) {
            char lower = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
            if (lower >= 'a' && lower <= 'z') {
                target += lower;
            }
        }
    }

    std::vector<CorrectionCandidate> candidates;

    if (target.empty() || trie.getRoot() == nullptr) {
        if (outExecutionTimeUs) {
            auto endTime = std::chrono::high_resolution_clock::now();
            *outExecutionTimeUs = std::chrono::duration<double, std::micro>(endTime - startTime).count();
        }
        return candidates;
    }

    // Initialize DP row 0 for empty prefix: [0, 1, 2, ..., |target|]
    std::vector<int> initialRow(target.size() + 1);
    for (size_t j = 0; j <= target.size(); ++j) {
        initialRow[j] = static_cast<int>(j);
    }

    // If empty string is somehow in dictionary (distance = target.size())
    const TrieNode* root = trie.getRoot();
    if (root->isEndOfWord && static_cast<int>(target.size()) <= maxDistance) {
        candidates.emplace_back("", static_cast<int>(target.size()), root->frequency);
    }

    // Recursively explore all 26 child branches from root
    for (int i = 0; i < 26; ++i) {
        if (root->children[i]) {
            char ch = static_cast<char>('a' + i);
            searchTrieDP(root->children[i].get(), ch, target, maxDistance, initialRow, candidates);
        }
    }

    // Candidate Ranking:
    // 1. Primary: editDistance ASCENDING (closest edit distance first)
    // 2. Secondary: frequency DESCENDING (more popular words first)
    // 3. Tertiary: alphabetical ASCENDING (tie-break)
    auto comparator = [](const CorrectionCandidate& a, const CorrectionCandidate& b) {
        if (a.editDistance != b.editDistance) {
            return a.editDistance < b.editDistance; // Smallest edit distance first
        }
        if (a.frequency != b.frequency) {
            return a.frequency > b.frequency;       // Highest frequency first
        }
        return a.word < b.word;                     // Alphabetical tie-break
    };

    if (candidates.size() > limit) {
        std::partial_sort(candidates.begin(), candidates.begin() + limit, candidates.end(), comparator);
        candidates.resize(limit);
    } else {
        std::sort(candidates.begin(), candidates.end(), comparator);
    }

    if (outExecutionTimeUs) {
        auto endTime = std::chrono::high_resolution_clock::now();
        *outExecutionTimeUs = std::chrono::duration<double, std::micro>(endTime - startTime).count();
    }

    return candidates;
}

} // namespace dsa
