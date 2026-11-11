/**
 * @file Benchmark.cpp
 * @brief Implementation of benchmark execution, metrics collection, and CSV/JSON export.
 */

#include "Benchmark.h"
#include "Trie.h"
#include "Autocomplete.h"
#include "EditDistance.h"
#include "DictionaryScanner.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <chrono>

namespace dsa {

std::vector<BenchmarkRecord> BenchmarkRunner::runFullSuite(
    const std::vector<std::string>& datasetPaths,
    int trials,
    const std::string& outputCsvPath,
    const std::string& outputJsonPath
) {
    std::vector<BenchmarkRecord> records;

    const std::vector<std::string> prefixQueries = {
        "pro", "app", "comp", "data", "net", "sys", "mach", "alg", "soft", "dev"
    };

    const std::vector<std::string> typoQueries = {
        "pythn", "appl", "machne", "computr", "progrmming"
    };

    std::cout << "\n=========================================================================================\n";
    std::cout << "                 DSA-3 PERFORMANCE BENCHMARK SUITE: TRIE VS LINEAR SEARCH               \n";
    std::cout << "=========================================================================================\n";

    for (const auto& path : datasetPaths) {
        BenchmarkRecord record{};

        // 1. Measure Linear Dictionary Loading
        DictionaryScanner scanner;
        auto loadStart = std::chrono::high_resolution_clock::now();
        if (!scanner.loadFromFile(path)) {
            std::cerr << "Warning: Failed to load dataset: " << path << std::endl;
            continue;
        }
        auto loadEnd = std::chrono::high_resolution_clock::now();
        record.loadTimeMs = std::chrono::duration<double, std::milli>(loadEnd - loadStart).count();
        record.datasetSize = scanner.size();

        // 2. Measure Trie Construction Time
        Trie trie;
        auto trieBuildStart = std::chrono::high_resolution_clock::now();
        for (const auto& entry : scanner.getEntries()) {
            trie.insert(entry.first, entry.second);
        }
        auto trieBuildEnd = std::chrono::high_resolution_clock::now();
        record.trieBuildTimeMs = std::chrono::duration<double, std::milli>(trieBuildEnd - trieBuildStart).count();
        record.trieNodeCount = trie.getNodeCount();
        record.trieMemoryKb = static_cast<double>(trie.estimateMemoryUsage()) / 1024.0;

        Autocomplete autocomplete(trie);

        // 3. Measure Trie Prefix Search (averaged across multiple queries and repeated trials)
        double totalTrieSearchUs = 0.0;
        int searchIterations = 0;
        for (int t = 0; t < trials; ++t) {
            for (const auto& q : prefixQueries) {
                double timeUs = 0.0;
                autocomplete.getSuggestions(q, 5, &timeUs);
                totalTrieSearchUs += timeUs;
                searchIterations++;
            }
        }
        record.trieSearchAvgUs = totalTrieSearchUs / std::max(1, searchIterations);

        // 4. Measure Linear Prefix Search (averaged identically)
        double totalLinearSearchUs = 0.0;
        searchIterations = 0;
        for (int t = 0; t < trials; ++t) {
            for (const auto& q : prefixQueries) {
                double timeUs = 0.0;
                scanner.searchPrefix(q, 5, &timeUs);
                totalLinearSearchUs += timeUs;
                searchIterations++;
            }
        }
        record.linearSearchAvgUs = totalLinearSearchUs / std::max(1, searchIterations);
        record.searchSpeedup = record.linearSearchAvgUs / std::max(0.001, record.trieSearchAvgUs);

        // 5. Measure Typo Correction (Trie DP with Pruning vs Brute-force Linear DP)
        // Run fewer trials for linear typo correction since computing 50k Levenshtein distances is heavy
        int typoTrials = std::min(trials, 10);
        double totalTrieTypoUs = 0.0;
        int typoIterations = 0;
        for (int t = 0; t < typoTrials; ++t) {
            for (const auto& q : typoQueries) {
                double timeUs = 0.0;
                EditDistance::correctWord(trie, q, 2, 5, &timeUs);
                totalTrieTypoUs += timeUs;
                typoIterations++;
            }
        }
        record.trieTypoAvgUs = totalTrieTypoUs / std::max(1, typoIterations);

        double totalLinearTypoUs = 0.0;
        typoIterations = 0;
        for (int t = 0; t < typoTrials; ++t) {
            for (const auto& q : typoQueries) {
                double timeUs = 0.0;
                scanner.correctWord(q, 2, 5, &timeUs);
                totalLinearTypoUs += timeUs;
                typoIterations++;
            }
        }
        record.linearTypoAvgUs = totalLinearTypoUs / std::max(1, typoIterations);
        record.typoSpeedup = record.linearTypoAvgUs / std::max(0.001, record.trieTypoAvgUs);

        records.push_back(record);

        // Print progress row
        std::cout << "Words: " << std::setw(6) << record.datasetSize
                  << " | Trie Build: " << std::setw(6) << std::fixed << std::setprecision(2) << record.trieBuildTimeMs << " ms"
                  << " | Trie Search: " << std::setw(6) << std::setprecision(1) << record.trieSearchAvgUs << " us"
                  << " | Linear Search: " << std::setw(7) << std::setprecision(1) << record.linearSearchAvgUs << " us"
                  << " | Speedup: " << std::setw(5) << std::setprecision(1) << record.searchSpeedup << "x"
                  << " | Typo Speedup: " << std::setw(5) << std::setprecision(1) << record.typoSpeedup << "x\n";
    }

    std::cout << "=========================================================================================\n";

    exportToCsv(records, outputCsvPath);
    exportToJson(records, outputJsonPath);

    return records;
}

bool BenchmarkRunner::exportToCsv(const std::vector<BenchmarkRecord>& records, const std::string& filepath) {
    std::ofstream out(filepath);
    if (!out.is_open()) return false;

    out << "dataset_size,load_time_ms,trie_build_time_ms,trie_node_count,trie_memory_kb,"
        << "trie_search_avg_us,linear_search_avg_us,search_speedup,"
        << "trie_typo_avg_us,linear_typo_avg_us,typo_speedup\n";

    for (const auto& r : records) {
        out << r.datasetSize << ","
            << r.loadTimeMs << ","
            << r.trieBuildTimeMs << ","
            << r.trieNodeCount << ","
            << r.trieMemoryKb << ","
            << r.trieSearchAvgUs << ","
            << r.linearSearchAvgUs << ","
            << r.searchSpeedup << ","
            << r.trieTypoAvgUs << ","
            << r.linearTypoAvgUs << ","
            << r.typoSpeedup << "\n";
    }

    return true;
}

bool BenchmarkRunner::exportToJson(const std::vector<BenchmarkRecord>& records, const std::string& filepath) {
    std::ofstream out(filepath);
    if (!out.is_open()) return false;

    out << "[\n";
    for (size_t i = 0; i < records.size(); ++i) {
        const auto& r = records[i];
        out << "  {\n"
            << "    \"dataset_size\": " << r.datasetSize << ",\n"
            << "    \"load_time_ms\": " << r.loadTimeMs << ",\n"
            << "    \"trie_build_time_ms\": " << r.trieBuildTimeMs << ",\n"
            << "    \"trie_node_count\": " << r.trieNodeCount << ",\n"
            << "    \"trie_memory_kb\": " << r.trieMemoryKb << ",\n"
            << "    \"trie_search_avg_us\": " << r.trieSearchAvgUs << ",\n"
            << "    \"linear_search_avg_us\": " << r.linearSearchAvgUs << ",\n"
            << "    \"search_speedup\": " << r.searchSpeedup << ",\n"
            << "    \"trie_typo_avg_us\": " << r.trieTypoAvgUs << ",\n"
            << "    \"linear_typo_avg_us\": " << r.linearTypoAvgUs << ",\n"
            << "    \"typo_speedup\": " << r.typoSpeedup << "\n"
            << "  }" << (i + 1 < records.size() ? "," : "") << "\n";
    }
    out << "]\n";

    return true;
}

} // namespace dsa

