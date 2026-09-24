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

enum class BFSStopReason {
    None,
    Deadline,
    StateLimit,
    Exhausted
};

struct SolveResult {
    SolveStatus status;
    std::vector<Move> moves;
    std::size_t statesExplored;
    std::size_t statesStored;
    BFSStopReason stopReason;
};

// Searches by increasing solution length. A non-positive timeout gives the
// search no time to expand an unsolved state.
SolveResult solveBFS(const CubeState& start,
                     int maxDepth,
                     std::chrono::milliseconds timeout);

SolveResult solveBFS(const CubeState& start,
                     int maxDepth,
                     std::chrono::milliseconds timeout,
                     std::size_t maxStates);