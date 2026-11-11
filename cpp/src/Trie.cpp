/**
 * @file Trie.cpp
 * @brief Implementation of Trie (Prefix Tree) methods.
 */

#include "Trie.h"
#include <algorithm>
#include <queue>

namespace dsa {

Trie::Trie() : root(std::make_unique<TrieNode>()), wordCount(0), nodeCount(1) {}

bool Trie::insert(const std::string& rawWord, int freq) {
    if (rawWord.empty()) {
        return false;
    }

    TrieNode* current = root.get();
    std::string normalized = "";
    normalized.reserve(rawWord.size());

    for (char ch : rawWord) {
        int idx = charToIndex(ch);
        if (idx == -1) {
            // Ignore non-alphabetic characters for standard dictionary Trie
            continue;
        }
        normalized += static_cast<char>('a' + idx);

        if (!current->children[idx]) {
            current->children[idx] = std::make_unique<TrieNode>();
            nodeCount++;
        }
        current = current->children[idx].get();
    }

    if (normalized.empty()) {
        return false;
    }

    bool isNewWord = !current->isEndOfWord;
    if (isNewWord) {
        current->isEndOfWord = true;
        current->word = normalized;
        current->frequency = freq;
        wordCount++;
    } else {
        // Update frequency for duplicate entries (retain the higher or accumulate)
        current->frequency = std::max(current->frequency, freq);
    }

    return isNewWord;
}

bool Trie::search(const std::string& rawWord) const {
    const TrieNode* node = getNode(rawWord);
    return (node != nullptr && node->isEndOfWord);
}

bool Trie::startsWith(const std::string& prefix) const {
    return (getNode(prefix) != nullptr);
}

const TrieNode* Trie::getNode(const std::string& rawPrefix) const {
    if (rawPrefix.empty()) {
        return root.get();
    }

    const TrieNode* current = root.get();
    for (char ch : rawPrefix) {
        int idx = charToIndex(ch);
        if (idx == -1) {
            return nullptr; // Character not supported
        }
        if (!current->children[idx]) {
            return nullptr;
        }
        current = current->children[idx].get();
    }
    return current;
}

void Trie::collectSubtreeWords(const TrieNode* node, std::vector<std::pair<std::string, int>>& results) const {
    if (!node) return;

    if (node->isEndOfWord) {
        results.emplace_back(node->word, node->frequency);
    }

    for (int i = 0; i < 26; ++i) {
        if (node->children[i]) {
            collectSubtreeWords(node->children[i].get(), results);
        }
    }
}

std::vector<std::pair<std::string, int>> Trie::getWordsWithPrefix(const std::string& prefix) const {
    std::vector<std::pair<std::string, int>> results;
    const TrieNode* prefixNode = getNode(prefix);

    if (prefixNode != nullptr) {
        collectSubtreeWords(prefixNode, results);
    }

    return results;
}

size_t Trie::estimateMemoryUsage() const {
    // Memory for Trie instance + all TrieNodes + heap string capacity
    size_t total = sizeof(Trie);
    total += nodeCount * sizeof(TrieNode);
    // Rough estimate for string heap allocations at terminal nodes
    total += wordCount * (sizeof(std::string) + 16);
    return total;
}

void Trie::clear() {
    root = std::make_unique<TrieNode>();
    wordCount = 0;
    nodeCount = 1;
}

} // namespace dsa
