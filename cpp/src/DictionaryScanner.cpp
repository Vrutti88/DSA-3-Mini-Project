/**
 * @file DictionaryScanner.cpp
 * @brief Implementation of linear dictionary scanning baseline.
 */

#include "DictionaryScanner.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>

namespace dsa {

bool DictionaryScanner::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }

    dictionary.clear();
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::istringstream iss(line);
        std::string word;
        int freq = 1;

        // Handles format "word freq" or "word,freq"
        if (iss >> word) {
            size_t comma = word.find(',');
            if (comma != std::string::npos) {
                std::string freqStr = word.substr(comma + 1);
                word = word.substr(0, comma);
                try {
                    freq = std::stoi(freqStr);
                } catch (...) {
                    freq = 1;
                }
            } else {
                iss >> freq;
            }
            addWord(word, freq);
        }
    }

    return true;
}

void DictionaryScanner::addWord(const std::string& rawWord, int freq) {
    std::string word = "";
    for (char c : rawWord) {
        if (!std::isspace(static_cast<unsigned char>(c))) {
            char lower = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
            if (lower >= 'a' && lower <= 'z') {
                word += lower;
            }
        }
    }
    if (!word.empty()) {
        dictionary.emplace_back(std::move(word), freq);
    }
}

std::vector<std::pair<std::string, int>> DictionaryScanner::searchPrefix(
    const std::string& prefix,
    size_t limit,
    double* outExecutionTimeUs
) const {
    auto startTime = std::chrono::high_resolution_clock::now();

    std::string target = "";
    for (char c : prefix) {
        if (!std::isspace(static_cast<unsigned char>(c))) {
            char lower = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
            if (lower >= 'a' && lower <= 'z') target += lower;
        }
    }

    std::vector<std::pair<std::string, int>> matches;

    if (!target.empty()) {
        size_t pLen = target.size();
        // Linear scan over every single word in the dictionary
        for (const auto& entry : dictionary) {
            if (entry.first.size() >= pLen && entry.first.compare(0, pLen, target) == 0) {
                matches.push_back(entry);
            }
        }

        auto comparator = [](const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) {
            if (a.second != b.second) return a.second > b.second;
            return a.first < b.first;
        };

        if (matches.size() > limit) {
            std::partial_sort(matches.begin(), matches.begin() + limit, matches.end(), comparator);
            matches.resize(limit);
        } else {
            std::sort(matches.begin(), matches.end(), comparator);
        }
    }

    if (outExecutionTimeUs) {
        auto endTime = std::chrono::high_resolution_clock::now();
        *outExecutionTimeUs = std::chrono::duration<double, std::micro>(endTime - startTime).count();
    }

    return matches;
}

std::vector<CorrectionCandidate> DictionaryScanner::correctWord(
    const std::string& query,
    int maxDistance,
    size_t limit,
    double* outExecutionTimeUs
) const {
    auto startTime = std::chrono::high_resolution_clock::now();

    std::string target = "";
    for (char c : query) {
        if (!std::isspace(static_cast<unsigned char>(c))) {
            char lower = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
            if (lower >= 'a' && lower <= 'z') target += lower;
        }
    }

    std::vector<CorrectionCandidate> candidates;

    if (!target.empty()) {
        // Brute-force linear scan: compute DP distance against EVERY dictionary word
        for (const auto& entry : dictionary) {
            // Quick length filter optimization: if length difference > maxDistance, impossible
            int lenDiff = std::abs(static_cast<int>(entry.first.size()) - static_cast<int>(target.size()));
            if (lenDiff > maxDistance) continue;

            int dist = EditDistance::calculateDistance(entry.first, target);
            if (dist <= maxDistance) {
                candidates.emplace_back(entry.first, dist, entry.second);
            }
        }

        auto comparator = [](const CorrectionCandidate& a, const CorrectionCandidate& b) {
            if (a.editDistance != b.editDistance) return a.editDistance < b.editDistance;
            if (a.frequency != b.frequency) return a.frequency > b.frequency;
            return a.word < b.word;
        };

        if (candidates.size() > limit) {
            std::partial_sort(candidates.begin(), candidates.begin() + limit, candidates.end(), comparator);
            candidates.resize(limit);
        } else {
            std::sort(candidates.begin(), candidates.end(), comparator);
        }
    }

    if (outExecutionTimeUs) {
        auto endTime = std::chrono::high_resolution_clock::now();
        *outExecutionTimeUs = std::chrono::duration<double, std::micro>(endTime - startTime).count();
    }

    return candidates;
}

} // namespace dsa

