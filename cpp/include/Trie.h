/**
 * @file Trie.h
 * @brief High-performance Trie (Prefix Tree) Data Structure in C++17.
 * 
 * Part of DSA-3 Mini Project: Smart Autocomplete and Typo Correction.
 * 
 * Design Details:
 * - Each node represents a single character along word paths.
 * - Nodes use fixed-size std::array of 26 pointers for lowercase English letters (a-z),
 *   achieving O(1) branch lookups with optimal CPU cache locality.
 * - Tracks end-of-word flags, word frequencies, and complete terminal strings.
 * - Supports prefix matching, subtree DFS traversal, frequency accounting, and memory profiling.
 */

#ifndef TRIE_H
#define TRIE_H

#include <string>
#include <vector>
#include <memory>
#include <array>
#include <utility>
#include <cctype>

namespace dsa {

/**
 * @brief Structure representing a single node in the Trie.
 */
struct TrieNode {
    // Array of 26 child pointers corresponding to 'a' through 'z'
    std::array<std::unique_ptr<TrieNode>, 26> children;
    
    // True if this node represents the final character of a valid dictionary word
    bool isEndOfWord;
    
    // Word frequency count (used for ranking suggestions)
    int frequency;
    
    // Exact word stored at terminal node for O(1) string retrieval during traversal
    std::string word;

    TrieNode() : isEndOfWord(false), frequency(0), word("") {
        for (auto& child : children) {
            child = nullptr;
        }
    }
};

/**
 * @brief Trie class providing prefix search, insertion, and word collection.
 */
class Trie {
private:
    std::unique_ptr<TrieNode> root;
    size_t wordCount;
    size_t nodeCount;

    // Helper to map 'a'-'z' to index 0-25
    static inline int charToIndex(char c) {
        c = std::tolower(static_cast<unsigned char>(c));
        if (c >= 'a' && c <= 'z') {
            return c - 'a';
        }
        return -1; // Non-alphabetic character
    }

    // Helper to map index 0-25 back to 'a'-'z'
    static inline char indexToChar(int idx) {
        return static_cast<char>('a' + idx);
    }

    // Recursive helper to collect all terminal words under a given node
    void collectSubtreeWords(const TrieNode* node, std::vector<std::pair<std::string, int>>& results) const;

public:
    Trie();
    ~Trie() = default;

    // Disallow copy to avoid expensive deep copies; allow move
    Trie(const Trie&) = delete;
    Trie& operator=(const Trie&) = delete;
    Trie(Trie&&) noexcept = default;
    Trie& operator=(Trie&&) noexcept = default;

    /**
     * @brief Inserts a word with associated frequency into the Trie.
     * If the word already exists, updates frequency to max(existing, freq).
     * @param word String to insert (normalized to lowercase).
     * @param freq Frequency score of the word (default 1).
     * @return true if inserted as a new word, false if updated existing.
     */
    bool insert(const std::string& word, int freq = 1);

    /**
     * @brief Searches for a complete word in the Trie.
     * @param word Target word.
     * @return true if exact word exists and is marked terminal, false otherwise.
     */
    bool search(const std::string& word) const;

    /**
     * @brief Checks if any word in the Trie starts with the given prefix.
     * @param prefix Target prefix.
     * @return true if prefix exists, false otherwise.
     */
    bool startsWith(const std::string& prefix) const;

    /**
     * @brief Retrieves the pointer to the TrieNode representing the given prefix.
     * @param prefix Target prefix string.
     * @return Pointer to node if found, nullptr otherwise.
     */
    const TrieNode* getNode(const std::string& prefix) const;

    /**
     * @brief Retrieves the root node of the Trie (used in DP-based traversal).
     * @return Raw pointer to root TrieNode.
     */
    const TrieNode* getRoot() const { return root.get(); }

    /**
     * @brief Retrieves all words in the Trie starting with the given prefix.
     * @param prefix Target prefix.
     * @return Vector of pairs (word, frequency).
     */
    std::vector<std::pair<std::string, int>> getWordsWithPrefix(const std::string& prefix) const;

    /**
     * @brief Returns total number of unique valid words stored.
     */
    size_t getWordCount() const { return wordCount; }

    /**
     * @brief Returns total number of nodes allocated in the Trie.
     */
    size_t getNodeCount() const { return nodeCount; }

    /**
     * @brief Estimates memory usage of the Trie in bytes.
     */
    size_t estimateMemoryUsage() const;

    /**
     * @brief Clears all nodes from the Trie and resets counters.
     */
    void clear();
};

} // namespace dsa

#endif // TRIE_H
