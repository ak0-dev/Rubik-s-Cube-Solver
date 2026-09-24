#include "SolverIDAStar.hpp"

#include "Heuristic.hpp"

#include <array>
#include <limits>

namespace {

constexpr int Found = -1;
constexpr int TimedOut = -2;
constexpr int Infinity = std::numeric_limits<int>::max();

constexpr std::array<Move, 18> allMoves{
    Move::U, Move::UPrime, Move::U2,
    Move::D, Move::DPrime, Move::D2,
    Move::L, Move::LPrime, Move::L2,
    Move::R, Move::RPrime, Move::R2,
    Move::F, Move::FPrime, Move::F2,
    Move::B, Move::BPrime, Move::B2,
};

int faceOf(Move move) {
    switch (move) {
    case Move::U:
    case Move::UPrime:
    case Move::U2:
        return 0;
    case Move::D:
    case Move::DPrime:
    case Move::D2:
        return 1;
    case Move::L:
    case Move::LPrime:
    case Move::L2:
        return 2;
    case Move::R:
    case Move::RPrime:
    case Move::R2:
        return 3;
    case Move::F:
    case Move::FPrime:
    case Move::F2:
        return 4;
    case Move::B:
    case Move::BPrime:
    case Move::B2:
        return 5;
    }

    return -1;
}

Move inverseOf(Move move) {
    switch (move) {
    case Move::U: return Move::UPrime;
    case Move::UPrime: return Move::U;
    case Move::U2: return Move::U2;
    case Move::D: return Move::DPrime;
    case Move::DPrime: return Move::D;
    case Move::D2: return Move::D2;
    case Move::L: return Move::LPrime;
    case Move::LPrime: return Move::L;
    case Move::L2: return Move::L2;
    case Move::R: return Move::RPrime;
    case Move::RPrime: return Move::R;
    case Move::R2: return Move::R2;
    case Move::F: return Move::FPrime;
    case Move::FPrime: return Move::F;
    case Move::F2: return Move::F2;
    case Move::B: return Move::BPrime;
    case Move::BPrime: return Move::B;
    case Move::B2: return Move::B2;
    }

    return Move::U;
}

int search(const CubeState& state,
           int depth,
           int threshold,
           Move lastMove,
           bool hasLastMove,
           std::vector<Move>& path,
           std::vector<Move>& solution,
           std::size_t& nodesExplored,
            int maxDepth,
           const std::chrono::steady_clock::time_point* deadline) {
        if (deadline != nullptr && std::chrono::steady_clock::now() >= *deadline) {
         return TimedOut;
        }
    ++nodesExplored;

    const int estimate = depth + heuristic(state);
    if (estimate > threshold) {
        return estimate;
    }
    if (state.isSolved()) {
        solution = path;
        return Found;
    }
    if (depth >= maxDepth) {
        return Infinity;
    }

    int smallestExceededThreshold = Infinity;
    for (Move move : allMoves) {
        if (hasLastMove && move == inverseOf(lastMove)) {
            continue;
        }

        // Three consecutive turns of one face can always be represented more
        // directly by an equivalent one- or two-turn move.
        if (path.size() >= 2 &&
            faceOf(path[path.size() - 1]) == faceOf(move) &&
            faceOf(path[path.size() - 2]) == faceOf(move)) {
            continue;
        }

        CubeState nextState = state;
        applyMove(nextState, move);
        path.push_back(move);
        const int nextThreshold = search(nextState,
                                         depth + 1,
                                         threshold,
                                         move,
                                         true,
                                         path,
                                         solution,
                                         nodesExplored,
                                         maxDepth,
                                         deadline);
        path.pop_back();

        if (nextThreshold == Found) {
            return Found;
        }
        if (nextThreshold == TimedOut) {
            return TimedOut;
        }
        if (nextThreshold < smallestExceededThreshold) {
            smallestExceededThreshold = nextThreshold;
        }
    }

    return smallestExceededThreshold;
}

}  // namespace

IDAStarResult solveIDAStarInternal(
    const CubeState& start,
    int maxDepth,
    const std::chrono::steady_clock::time_point* deadline) {
    IDAStarResult result{false, {}, 0, false};
    if (maxDepth < 0) {
        return result;
    }

    int threshold = heuristic(start);
    std::vector<Move> path;

    while (threshold <= maxDepth && threshold != Infinity) {
        const int nextThreshold = search(start,
                                         0,
                                         threshold,
                                         Move::U,
                                         false,
                                         path,
                                         result.moves,
                                         result.nodesExplored,
                                         maxDepth,
                                         deadline);
        if (nextThreshold == Found) {
            result.found = true;
            return result;
        }
        if (nextThreshold == TimedOut) {
            result.timedOut = true;
            return result;
        }
        if (nextThreshold == Infinity) {
            return result;
        }
        threshold = nextThreshold;
    }

    return result;
}

IDAStarResult solveIDAStar(const CubeState& start, int maxDepth) {
    return solveIDAStarInternal(start, maxDepth, nullptr);
}

IDAStarResult solveIDAStar(const CubeState& start,
                           int maxDepth,
                           std::chrono::milliseconds timeout) {
    const auto deadline = std::chrono::steady_clock::now() + timeout;
    return solveIDAStarInternal(start, maxDepth, &deadline);
}