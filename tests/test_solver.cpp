#include "Cube.hpp"
#include "Scrambler.hpp"
#include "SolverBFS.hpp"
#include "SolverIDAStar.hpp"

#include <chrono>
#include <iostream>
#include <random>

namespace {

bool appliesSolution(const CubeState& scrambled,
                     const std::vector<Move>& solution) {
    CubeState state = scrambled;
    for (Move move : solution) {
        applyMove(state, move);
    }
    return state.isSolved();
}

}  // namespace

int main() {
    constexpr int testCount = 20;
    std::mt19937 generator(20260913);
    std::uniform_int_distribution<int> depthDistribution(3, 6);

    int bfsPassed = 0;
    int idaPassed = 0;

    for (int test = 0; test < testCount; ++test) {
        const int depth = depthDistribution(generator);
        CubeState scrambled;
        scramble(scrambled, depth);

        const SolveResult bfsResult = solveBFS(
            scrambled, 7, std::chrono::seconds(15));
        if (bfsResult.status == SolveStatus::Solved &&
            appliesSolution(scrambled, bfsResult.moves)) {
            ++bfsPassed;
        }

        const IDAStarResult idaResult = solveIDAStar(
            scrambled, 7, std::chrono::seconds(15));
        if (idaResult.found && !idaResult.timedOut &&
            appliesSolution(scrambled, idaResult.moves)) {
            ++idaPassed;
        }
    }

    std::cout << bfsPassed << '/' << testCount << " BFS passed\n";
    std::cout << idaPassed << '/' << testCount << " IDA* passed\n";

    return bfsPassed == testCount && idaPassed == testCount ? 0 : 1;
}