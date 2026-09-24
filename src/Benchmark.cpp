#include "Benchmark.hpp"

#include "Scrambler.hpp"
#include "SolverBFS.hpp"
#include "SolverIDAStar.hpp"

#include <array>
#include <fstream>
#include <iomanip>
#include <iostream>

namespace {

bool solves(const CubeState& scrambled, const std::vector<Move>& moves) {
    CubeState state = scrambled;
    for (Move move : moves) {
        applyMove(state, move);
    }
    return state.isSolved();
}

std::string bfsStatus(const SolveResult& result, bool verified) {
    if (result.status == SolveStatus::Timeout) {
        return "timeout";
    }
    if (result.status == SolveStatus::Solved && verified) {
        return "success";
    }
    if (result.status == SolveStatus::Solved) {
        return "verification_failed";
    }
    return "not_found";
}

std::string idaStatus(const IDAStarResult& result, bool verified) {
    if (result.timedOut) {
        return "timeout";
    }
    if (result.found && verified) {
        return "success";
    }
    if (result.found) {
        return "verification_failed";
    }
    return "not_found";
}

}  // namespace

std::vector<BenchmarkResult> runBenchmark(const std::string& csvPath,
                                           std::chrono::milliseconds timeout) {
    constexpr std::array<int, 4> scrambleDepths{3, 5, 7, 9};
    std::vector<BenchmarkResult> results;
    results.reserve(scrambleDepths.size() * 2);

    for (int depth : scrambleDepths) {
        CubeState scrambled;
        scramble(scrambled, depth);

        const auto bfsStart = std::chrono::steady_clock::now();
        const SolveResult bfsResult = solveBFS(scrambled, depth, timeout);
        const auto bfsEnd = std::chrono::steady_clock::now();
        const bool bfsVerified = bfsResult.status == SolveStatus::Solved &&
                                 solves(scrambled, bfsResult.moves);
        const std::chrono::duration<double, std::milli> bfsElapsed = bfsEnd - bfsStart;
        results.push_back({depth,
                           "BFS",
                           bfsStatus(bfsResult, bfsVerified),
                           bfsElapsed.count(),
                           bfsResult.statesExplored,
                           bfsResult.moves.size()});

        const auto idaStart = std::chrono::steady_clock::now();
        const IDAStarResult idaResult = solveIDAStar(scrambled, depth, timeout);
        const auto idaEnd = std::chrono::steady_clock::now();
        const bool idaVerified = idaResult.found && solves(scrambled, idaResult.moves);
        const std::chrono::duration<double, std::milli> idaElapsed = idaEnd - idaStart;
        results.push_back({depth,
                           "IDA*",
                           idaStatus(idaResult, idaVerified),
                           idaElapsed.count(),
                           idaResult.nodesExplored,
                           idaResult.moves.size()});
    }

    std::cout << "\nBenchmark results\n"
              << std::left << std::setw(8) << "Depth"
              << std::setw(8) << "Solver"
              << std::setw(20) << "Status"
              << std::setw(16) << "Time (ms)"
              << std::setw(18) << "Nodes"
              << "Solution length\n";
    std::cout << std::string(82, '-') << '\n';
    for (const BenchmarkResult& result : results) {
        std::cout << std::left << std::setw(8) << result.scrambleDepth
                  << std::setw(8) << result.solver
                  << std::setw(20) << result.status
                  << std::setw(16) << std::fixed << std::setprecision(3)
                  << result.timeMilliseconds
                  << std::setw(18) << result.nodesExplored
                  << result.solutionLength << '\n';
    }

    std::ofstream csv(csvPath);
    if (csv) {
        csv << "scramble_depth,solver,status,time_ms,nodes_explored,solution_length\n";
        for (const BenchmarkResult& result : results) {
            csv << result.scrambleDepth << ','
                << result.solver << ','
                << result.status << ','
                << std::fixed << std::setprecision(3) << result.timeMilliseconds << ','
                << result.nodesExplored << ','
                << result.solutionLength << '\n';
        }
    } else {
        std::cerr << "Unable to write benchmark CSV: " << csvPath << '\n';
    }

    return results;
}