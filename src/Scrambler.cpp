#include "Scrambler.hpp"

#include <array>
#include <random>

namespace {

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

}  // namespace

std::vector<Move> scramble(CubeState& state, int numMoves) {
    std::random_device seed;
    std::mt19937 generator(seed());
    std::vector<Move> appliedMoves;
    appliedMoves.reserve(numMoves > 0 ? static_cast<std::size_t>(numMoves) : 0);

    int previousFace = -1;
    int faceBeforePrevious = -1;

    for (int moveNumber = 0; moveNumber < numMoves; ++moveNumber) {
        std::vector<Move> candidates;
        candidates.reserve(allMoves.size());

        for (Move candidate : allMoves) {
            const int candidateFace = faceOf(candidate);
            const bool undoesPrevious =
                !appliedMoves.empty() && candidate == inverseOf(appliedMoves.back());
            const bool wouldTurnFaceThreeTimes =
                candidateFace == previousFace && previousFace == faceBeforePrevious;

            if (!undoesPrevious && !wouldTurnFaceThreeTimes) {
                candidates.push_back(candidate);
            }
        }

        std::uniform_int_distribution<std::size_t> distribution(
            0, candidates.size() - 1);
        const Move selectedMove = candidates[distribution(generator)];

        applyMove(state, selectedMove);
        appliedMoves.push_back(selectedMove);
        faceBeforePrevious = previousFace;
        previousFace = faceOf(selectedMove);
    }

    return appliedMoves;
}