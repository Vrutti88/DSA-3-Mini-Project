/**
 * @file main.cpp
 * @brief Master CLI and JSON Protocol Engine for DSA-3 Mini Project.
 * 
 * Exposes Autocomplete, DP Typo Correction, Linear Comparison, and Benchmarks
 * via clean JSON communication to Python Flask and command-line callers.
 */

#include "Trie.h"
#include "Autocomplete.h"
#include "EditDistance.h"
#include "DictionaryScanner.h"
#include "Benchmark.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>

// Simple JSON string escaping helper
static std::string escapeJson(const std::string& s) {
    std::string out;
    for (char c : s) {
        if (c == '"') out += "\\\"";
        else if (c == '\\') out += "\\\\";
        else if (c == '\b') out += "\\b";
        else if (c == '\f') out += "\\f";
        else if (c == '\n') out += "\\n";
        else if (c == '\r') out += "\\r";
        else if (c == '\t') out += "\\t";
        else out += c;
    }
    return out;
}

// Global data structures
static dsa::Trie g_trie;
static dsa::DictionaryScanner g_scanner;
static std::string g_dictPath = "data/dictionary.txt";

static bool loadDictionary(const std::string& path) {
    g_dictPath = path;
    g_trie.clear();
    if (!g_scanner.loadFromFile(path)) {
        return false;
    }
    for (const auto& entry : g_scanner.getEntries()) {
        g_trie.insert(entry.first, entry.second);
    }
    return true;
}

static void outputError(const std::string& message) {
    std::cout << "{\"status\":\"error\",\"message\":\"" << escapeJson(message) << "\"}\n";
}

static void handleAutocomplete(const std::string& prefix, size_t limit) {
    dsa::Autocomplete ac(g_trie);
    double timeUs = 0.0;
    auto suggestions = ac.getSuggestions(prefix, limit, &timeUs);

    std::cout << "{\n"
              << "  \"status\": \"success\",\n"
              << "  \"query\": \"" << escapeJson(prefix) << "\",\n"
              << "  \"count\": " << suggestions.size() << ",\n"
              << "  \"execution_time_us\": " << std::fixed << std::setprecision(2) << timeUs << ",\n"
              << "  \"algorithm\": \"trie_prefix_search\",\n"
              << "  \"results\": [\n";
    for (size_t i = 0; i < suggestions.size(); ++i) {
        std::cout << "    {\"word\": \"" << escapeJson(suggestions[i].word) 
                  << "\", \"frequency\": " << suggestions[i].frequency << "}"
                  << (i + 1 < suggestions.size() ? "," : "") << "\n";
    }
    std::cout << "  ]\n}\n";
}

static void handleCorrect(const std::string& query, int maxDist, size_t limit) {
    double timeUs = 0.0;
    auto candidates = dsa::EditDistance::correctWord(g_trie, query, maxDist, limit, &timeUs);

    std::cout << "{\n"
              << "  \"status\": \"success\",\n"
              << "  \"query\": \"" << escapeJson(query) << "\",\n"
              << "  \"max_distance\": " << maxDist << ",\n"
              << "  \"count\": " << candidates.size() << ",\n"
              << "  \"execution_time_us\": " << std::fixed << std::setprecision(2) << timeUs << ",\n"
              << "  \"algorithm\": \"trie_dp_traversal\",\n"
              << "  \"results\": [\n";
    for (size_t i = 0; i < candidates.size(); ++i) {
        std::cout << "    {\"word\": \"" << escapeJson(candidates[i].word) 
                  << "\", \"edit_distance\": " << candidates[i].editDistance
                  << ", \"frequency\": " << candidates[i].frequency << "}"
                  << (i + 1 < candidates.size() ? "," : "") << "\n";
    }
    std::cout << "  ]\n}\n";
}

static void handleSearch(const std::string& query, size_t limit) {
    bool exact = g_trie.search(query);
    dsa::Autocomplete ac(g_trie);
    double acTimeUs = 0.0;
    auto suggestions = ac.getSuggestions(query, limit, &acTimeUs);

    double typoTimeUs = 0.0;
    std::vector<dsa::CorrectionCandidate> corrections;
    // If no exact match or very few suggestions, run typo correction
    if (!exact || suggestions.size() < limit) {
        corrections = dsa::EditDistance::correctWord(g_trie, query, 2, limit, &typoTimeUs);
    }

    std::cout << "{\n"
              << "  \"status\": \"success\",\n"
              << "  \"query\": \"" << escapeJson(query) << "\",\n"
              << "  \"exact_match\": " << (exact ? "true" : "false") << ",\n"
              << "  \"autocomplete_time_us\": " << std::fixed << std::setprecision(2) << acTimeUs << ",\n"
              << "  \"typo_time_us\": " << std::fixed << std::setprecision(2) << typoTimeUs << ",\n"
              << "  \"autocomplete_results\": [\n";
    for (size_t i = 0; i < suggestions.size(); ++i) {
        std::cout << "    {\"word\": \"" << escapeJson(suggestions[i].word) 
                  << "\", \"frequency\": " << suggestions[i].frequency << "}"
                  << (i + 1 < suggestions.size() ? "," : "") << "\n";
    }
    std::cout << "  ],\n  \"typo_corrections\": [\n";
    for (size_t i = 0; i < corrections.size(); ++i) {
        std::cout << "    {\"word\": \"" << escapeJson(corrections[i].word) 
                  << "\", \"edit_distance\": " << corrections[i].editDistance
                  << ", \"frequency\": " << corrections[i].frequency << "}"
                  << (i + 1 < corrections.size() ? "," : "") << "\n";
    }
    std::cout << "  ]\n}\n";
}

static void handleCompare(const std::string& prefix, size_t limit) {
    dsa::Autocomplete ac(g_trie);
    double trieTimeUs = 0.0;
    auto trieResults = ac.getSuggestions(prefix, limit, &trieTimeUs);

    double linearTimeUs = 0.0;
    auto linearResults = g_scanner.searchPrefix(prefix, limit, &linearTimeUs);

    double speedup = linearTimeUs / std::max(0.001, trieTimeUs);

    std::cout << "{\n"
              << "  \"status\": \"success\",\n"
              << "  \"query\": \"" << escapeJson(prefix) << "\",\n"
              << "  \"trie_time_us\": " << std::fixed << std::setprecision(2) << trieTimeUs << ",\n"
              << "  \"linear_time_us\": " << std::fixed << std::setprecision(2) << linearTimeUs << ",\n"
              << "  \"speedup_factor\": " << std::fixed << std::setprecision(2) << speedup << ",\n"
              << "  \"trie_results\": [\n";
    for (size_t i = 0; i < trieResults.size(); ++i) {
        std::cout << "    {\"word\": \"" << escapeJson(trieResults[i].word) 
                  << "\", \"frequency\": " << trieResults[i].frequency << "}"
                  << (i + 1 < trieResults.size() ? "," : "") << "\n";
    }
    std::cout << "  ],\n  \"linear_results\": [\n";
    for (size_t i = 0; i < linearResults.size(); ++i) {
        std::cout << "    {\"word\": \"" << escapeJson(linearResults[i].first) 
                  << "\", \"frequency\": " << linearResults[i].second << "}"
                  << (i + 1 < linearResults.size() ? "," : "") << "\n";
    }
    std::cout << "  ]\n}\n";
}

static void handleStats() {
    size_t words = g_trie.getWordCount();
    size_t nodes = g_trie.getNodeCount();
    double memKb = static_cast<double>(g_trie.estimateMemoryUsage()) / 1024.0;

    std::cout << "{\n"
              << "  \"status\": \"success\",\n"
              << "  \"dictionary_file\": \"" << escapeJson(g_dictPath) << "\",\n"
              << "  \"word_count\": " << words << ",\n"
              << "  \"trie_node_count\": " << nodes << ",\n"
              << "  \"trie_memory_kb\": " << std::fixed << std::setprecision(2) << memKb << ",\n"
              << "  \"trie_memory_mb\": " << std::fixed << std::setprecision(3) << (memKb / 1024.0) << "\n"
              << "}\n";
}

static void handleDPMatrix(const std::string& s1, const std::string& s2) {
    auto matrix = dsa::EditDistance::getDPMatrix(s1, s2);
    int dist = matrix.empty() ? 0 : matrix.back().back();

    std::cout << "{\n"
              << "  \"status\": \"success\",\n"
              << "  \"s1\": \"" << escapeJson(s1) << "\",\n"
              << "  \"s2\": \"" << escapeJson(s2) << "\",\n"
              << "  \"distance\": " << dist << ",\n"
              << "  \"rows\": " << matrix.size() << ",\n"
              << "  \"cols\": " << (matrix.empty() ? 0 : matrix[0].size()) << ",\n"
              << "  \"matrix\": [\n";
    for (size_t i = 0; i < matrix.size(); ++i) {
        std::cout << "    [";
        for (size_t j = 0; j < matrix[i].size(); ++j) {
            std::cout << matrix[i][j] << (j + 1 < matrix[i].size() ? ", " : "");
        }
        std::cout << "]" << (i + 1 < matrix.size() ? "," : "") << "\n";
    }
    std::cout << "  ]\n}\n";
}

static void handleBenchmark() {
    std::vector<std::string> paths = {
        "data/dict_1k.txt",
        "data/dict_5k.txt",
        "data/dict_10k.txt",
        "data/dict_25k.txt",
        "data/dict_50k.txt"
    };

    auto records = dsa::BenchmarkRunner::runFullSuite(
        paths, 50, "benchmarks/results/benchmark_results.csv", "benchmarks/results/benchmark_results.json"
    );

    std::cout << "{\n"
              << "  \"status\": \"success\",\n"
              << "  \"message\": \"Benchmark suite completed successfully\",\n"
              << "  \"csv_path\": \"benchmarks/results/benchmark_results.csv\",\n"
              << "  \"json_path\": \"benchmarks/results/benchmark_results.json\",\n"
              << "  \"dataset_count\": " << records.size() << "\n"
              << "}\n";
}

static void runDaemonMode() {
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) continue;
        if (line == "EXIT" || line == "QUIT") break;

        std::istringstream iss(line);
        std::string command;
        iss >> command;

        if (command == "AUTOCOMPLETE") {
            std::string prefix;
            size_t limit = 5;
            iss >> prefix >> limit;
            handleAutocomplete(prefix, limit);
        } else if (command == "CORRECT") {
            std::string query;
            int maxDist = 2;
            size_t limit = 5;
            iss >> query >> maxDist >> limit;
            handleCorrect(query, maxDist, limit);
        } else if (command == "SEARCH") {
            std::string query;
            size_t limit = 5;
            iss >> query >> limit;
            handleSearch(query, limit);
        } else if (command == "COMPARE") {
            std::string prefix;
            size_t limit = 5;
            iss >> prefix >> limit;
            handleCompare(prefix, limit);
        } else if (command == "STATS") {
            handleStats();
        } else if (command == "DPMATRIX") {
            std::string s1, s2;
            iss >> s1 >> s2;
            handleDPMatrix(s1, s2);
        } else if (command == "BENCHMARK") {
            handleBenchmark();
        } else {
            outputError("Unknown daemon command: " + command);
        }
        std::cout << "---END---\n" << std::flush;
    }
}

int main(int argc, char* argv[]) {
    std::string dictPath = "data/dictionary.txt";
    std::string command = "";
    std::string query = "";
    std::string extra = "";
    size_t limit = 5;
    int maxDist = 2;
    bool daemonMode = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--dict" && i + 1 < argc) {
            dictPath = argv[++i];
        } else if (arg == "--autocomplete" && i + 1 < argc) {
            command = "autocomplete";
            query = argv[++i];
        } else if (arg == "--correct" && i + 1 < argc) {
            command = "correct";
            query = argv[++i];
        } else if (arg == "--search" && i + 1 < argc) {
            command = "search";
            query = argv[++i];
        } else if (arg == "--compare" && i + 1 < argc) {
            command = "compare";
            query = argv[++i];
        } else if (arg == "--dp-matrix" && i + 2 < argc) {
            command = "dp-matrix";
            query = argv[++i];
            extra = argv[++i];
        } else if (arg == "--stats") {
            command = "stats";
        } else if (arg == "--benchmark") {
            command = "benchmark";
        } else if (arg == "--limit" && i + 1 < argc) {
            limit = std::stoul(argv[++i]);
        } else if (arg == "--max-dist" && i + 1 < argc) {
            maxDist = std::stoi(argv[++i]);
        } else if (arg == "--serve" || arg == "--daemon") {
            daemonMode = true;
        }
    }

    if (!loadDictionary(dictPath)) {
        outputError("Could not load dictionary file at: " + dictPath);
        return 1;
    }

    if (daemonMode) {
        runDaemonMode();
        return 0;
    }

    if (command == "autocomplete") {
        handleAutocomplete(query, limit);
    } else if (command == "correct") {
        handleCorrect(query, maxDist, limit);
    } else if (command == "search") {
        handleSearch(query, limit);
    } else if (command == "compare") {
        handleCompare(query, limit);
    } else if (command == "stats") {
        handleStats();
    } else if (command == "dp-matrix") {
        handleDPMatrix(query, extra);
    } else if (command == "benchmark") {
        handleBenchmark();
    } else {
        std::cout << "Smart Autocomplete & Typo Correction Engine (C++17)\n"
                  << "Usage:\n"
                  << "  --dict <path>           Dictionary file (default: data/dictionary.txt)\n"
                  << "  --autocomplete <prefix> Suggest completions for prefix\n"
                  << "  --correct <word>        Find typo corrections using Trie DP\n"
                  << "  --search <query>        Perform search + autocomplete + correction\n"
                  << "  --compare <prefix>      Compare Trie vs Linear Scan for prefix\n"
                  << "  --dp-matrix <s1> <s2>   Output full Levenshtein DP Matrix\n"
                  << "  --stats                 Display dictionary & Trie memory statistics\n"
                  << "  --benchmark             Run full benchmark suite across all sizes\n"
                  << "  --serve                 Start in persistent daemon mode\n";
    }

    return 0;
}

