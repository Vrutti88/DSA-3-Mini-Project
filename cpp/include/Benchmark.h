/**
 * @file Benchmark.h
 * @brief Algorithmic benchmarking suite for Trie vs Linear Search across dataset sizes.
 * 
 * Part of DSA-3 Mini Project: Smart Autocomplete and Typo Correction.
 */

#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <string>
#include <vector>

namespace dsa {

/**
 * @brief Structure holding measured benchmark results for a single dataset size.
 */
struct BenchmarkRecord {
    size_t datasetSize;
    double loadTimeMs;
    double trieBuildTimeMs;
    size_t trieNodeCount;
    double trieMemoryKb;
    double trieSearchAvgUs;
    double linearSearchAvgUs;
    double searchSpeedup;
    double trieTypoAvgUs;
    double linearTypoAvgUs;
    double typoSpeedup;
};

/**
 * @brief Benchmarking orchestrator executing identical workloads on Trie vs Linear structures.
 */
class BenchmarkRunner {
public:
    /**
     * @brief Executes full benchmark suite across all dataset sizes.
     * 
     * @param datasetPaths Paths to dictionary files (e.g. 1k, 5k, 10k, 25k, 50k).
     * @param trials Number of repetitions per query to eliminate OS jitter (default 50).
     * @param outputCsvPath Path to write CSV results.
     * @param outputJsonPath Path to write JSON results.
     * @return Vector of BenchmarkRecord results.
     */
    static std::vector<BenchmarkRecord> runFullSuite(
        const std::vector<std::string>& datasetPaths,
        int trials = 50,
        const std::string& outputCsvPath = "benchmarks/results/benchmark_results.csv",
        const std::string& outputJsonPath = "benchmarks/results/benchmark_results.json"
    );

    /**
     * @brief Exports results to CSV.
     */
    static bool exportToCsv(const std::vector<BenchmarkRecord>& records, const std::string& filepath);

    /**
     * @brief Exports results to JSON.
     */
    static bool exportToJson(const std::vector<BenchmarkRecord>& records, const std::string& filepath);
};

} // namespace dsa

#endif // BENCHMARK_H

