#pragma once

#include "Cube.hpp"
#include "MoveEngine.hpp"

#include <chrono>
#include <cstddef>
#include <vector>

struct IDAStarResult {
    bool found;
    std::vector<Move> moves;
    std::size_t nodesExplored;
    bool timedOut;
};

// Searches in increasing f = g + heuristic(state) thresholds until a solution
// is found or maxDepth prevents another iteration.
IDAStarResult solveIDAStar(const CubeState& start, int maxDepth);
IDAStarResult solveIDAStar(const CubeState& start,
                           int maxDepth,
                           std::chrono::milliseconds timeout);