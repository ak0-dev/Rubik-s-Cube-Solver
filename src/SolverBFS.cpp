#include "SolverBFS.hpp"

#include <array>
#include <deque>
#include <limits>
#include <unordered_set>

namespace {

constexpr std::array<Move, 18> allMoves{
    Move::U, Move::UPrime, Move::U2,
    Move::D, Move::DPrime, Move::D2,
    Move::L, Move::LPrime, Move::L2,
    Move::R, Move::RPrime, Move::R2,
    Move::F, Move::FPrime, Move::F2,
    Move::B, Move::BPrime, Move::B2,
};

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

struct CubeStateHash {
    std::size_t operator()(const CubeState& state) const {
        std::size_t hash = 0;
        const auto combine = [&hash](const auto& values) {
            for (int value : values) {
                hash ^= static_cast<std::size_t>(value + 1) +
                        static_cast<std::size_t>(0x9e3779b9) +
                        (hash << 6) + (hash >> 2);
            }
        };

        combine(state.cornerPerm);
        combine(state.cornerOrient);
        combine(state.edgePerm);
        combine(state.edgeOrient);
        return hash;
    }
};

struct SearchNode {
    CubeState state;
    std::vector<Move> moves;
    Move lastMove;
    bool hasLastMove;
};

}  // namespace

SolveResult solveBFS(const CubeState& start,
                     int maxDepth,
                     std::chrono::milliseconds timeout) {
    return solveBFS(start, maxDepth, timeout, std::numeric_limits<std::size_t>::max());
}

SolveResult solveBFS(const CubeState& start,
                     int maxDepth,
                     std::chrono::milliseconds timeout,
                     std::size_t maxStates) {
    SolveResult result{SolveStatus::NotFound, {}, 0, 0, BFSStopReason::None};
    const auto deadline = std::chrono::steady_clock::now() + timeout;

    if (start.isSolved()) {
        result.status = SolveStatus::Solved;
        result.stopReason = BFSStopReason::Exhausted;
        return result;
    }

    if (maxDepth < 0 || timeout.count() <= 0) {
        result.status = timeout.count() <= 0
                            ? SolveStatus::Timeout
                            : SolveStatus::NotFound;
        result.stopReason = BFSStopReason::Deadline;
        return result;
    }

    std::deque<SearchNode> queue;
    std::unordered_set<CubeState, CubeStateHash> visited;
    queue.push_back({start, {}, Move::U, false});
    visited.insert(start);
    result.statesStored = visited.size();

    while (!queue.empty()) {
        if (std::chrono::steady_clock::now() >= deadline) {
            result.status = SolveStatus::Timeout;
            result.statesStored = visited.size();
            result.stopReason = BFSStopReason::Deadline;
            return result;
        }

        if (visited.size() >= maxStates) {
            result.status = SolveStatus::Timeout;
            result.statesStored = visited.size();
            result.stopReason = BFSStopReason::StateLimit;
            return result;
        }

        SearchNode current = std::move(queue.front());
        queue.pop_front();
        ++result.statesExplored;

        if (current.moves.size() >= static_cast<std::size_t>(maxDepth)) {
            continue;
        }

        for (Move move : allMoves) {
            if (current.hasLastMove && move == inverseOf(current.lastMove)) {
                continue;
            }
            CubeState nextState = current.state;
            applyMove(nextState, move);
            if (!visited.insert(nextState).second) {
                continue;
            }

            std::vector<Move> nextMoves = current.moves;
            nextMoves.push_back(move);
            if (nextState.isSolved()) {
                result.status = SolveStatus::Solved;
                result.moves = std::move(nextMoves);
                result.statesStored = visited.size();
                result.stopReason = BFSStopReason::Exhausted;
                return result;
            }

            queue.push_back({std::move(nextState), std::move(nextMoves), move, true});
        }
    }

    result.statesStored = visited.size();
    result.stopReason = BFSStopReason::Exhausted;
    return result;
}