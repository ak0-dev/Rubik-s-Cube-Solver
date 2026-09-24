#pragma once

#include <chrono>
#include <cstddef>
#include <string>
#include <vector>

struct BenchmarkResult {
    int scrambleDepth;
    std::string solver;
    std::string status;
    double timeMilliseconds;
    std::size_t nodesExplored;
    std::size_t solutionLength;
};

std::vector<BenchmarkResult> runBenchmark(
    const std::string& csvPath = "benchmark_results.csv",
    std::chrono::milliseconds timeout = std::chrono::seconds(2));