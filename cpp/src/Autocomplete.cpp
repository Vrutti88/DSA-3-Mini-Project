/**
 * @file Autocomplete.cpp
 * @brief Implementation of Autocomplete logic with frequency and alphabetical tie-breaking.
 */

#include "Autocomplete.h"
#include <algorithm>
#include <chrono>

namespace dsa {

std::vector<AutocompleteResult> Autocomplete::getSuggestions(
    const std::string& prefix, 
    size_t limit,
    double* outExecutionTimeUs
) const {
    auto startTime = std::chrono::high_resolution_clock::now();

    // Sanitize input: strip whitespace and convert to lowercase
    std::string sanitized = "";
    sanitized.reserve(prefix.size());
    for (char c : prefix) {
        if (!std::isspace(static_cast<unsigned char>(c))) {
            sanitized += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        }
    }

    std::vector<AutocompleteResult> results;

    if (sanitized.empty()) {
        if (outExecutionTimeUs) {
            auto endTime = std::chrono::high_resolution_clock::now();
            *outExecutionTimeUs = std::chrono::duration<double, std::micro>(endTime - startTime).count();
        }
        return results;
    }

    // 1. Traverse to prefix node and collect all matching words in the subtree
    auto rawMatches = trie.getWordsWithPrefix(sanitized);

    if (rawMatches.empty()) {
        if (outExecutionTimeUs) {
            auto endTime = std::chrono::high_resolution_clock::now();
            *outExecutionTimeUs = std::chrono::duration<double, std::micro>(endTime - startTime).count();
        }
        return results;
    }

    // 2. Rank candidates:
    // Primary criterion: frequency descending (higher frequency first)
    // Secondary criterion: lexicographical order ascending (alphabetical tie-breaker)
    auto comparator = [](const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) {
        if (a.second != b.second) {
            return a.second > b.second; // Higher frequency first
        }
        return a.first < b.first;       // Alphabetical order for equal frequency
    };

    if (rawMatches.size() > limit) {
        // Use partial sort to find top 'limit' elements efficiently in O(N log K)
        std::partial_sort(rawMatches.begin(), rawMatches.begin() + limit, rawMatches.end(), comparator);
        rawMatches.resize(limit);
    } else {
        std::sort(rawMatches.begin(), rawMatches.end(), comparator);
    }

    // 3. Populate final results
    results.reserve(rawMatches.size());
    for (const auto& item : rawMatches) {
        results.emplace_back(item.first, item.second);
    }

    if (outExecutionTimeUs) {
        auto endTime = std::chrono::high_resolution_clock::now();
        *outExecutionTimeUs = std::chrono::duration<double, std::micro>(endTime - startTime).count();
    }

    return results;
}

} // namespace dsa
