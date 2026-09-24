#pragma once

#include "Cube.hpp"
#include "MoveEngine.hpp"

#include <chrono>
#include <cstddef>
#include <vector>

enum class SolveStatus {
    Solved,
    NotFound,
    Timeout
};

struct SolveResult {
    SolveStatus status;
    std::vector<Move> moves;
    std::size_t statesExplored;
};

// Searches by increasing solution length. A non-positive timeout gives the
// search no time to expand an unsolved state.
SolveResult solveBFS(const CubeState& start,
                     int maxDepth,
                     std::chrono::milliseconds timeout);