#include "MoveEngine.hpp"

#include <array>

namespace {

// Corners: URF, UFL, ULB, UBR, DFR, DLF, DBL, DRB.
// Edges: UR, UF, UL, UB, DR, DF, DL, DB, FR, FL, BL, BR.
// The cycle entries are source positions. A clockwise move sends each source
// piece to the next position in its cycle when viewed from that face.
template <std::size_t Size>
void cyclePieces(std::array<int, Size>& permutation,
                 std::array<int, Size>& orientation,
                 const std::array<std::size_t, 4>& cycle,
                 const std::array<int, 4>& orientationDelta,
                 int orientationModulus) {
    const auto oldPermutation = permutation;
    const auto oldOrientation = orientation;

    for (std::size_t index = 0; index < cycle.size(); ++index) {
        const std::size_t source = cycle[index];
        const std::size_t destination = cycle[(index + 1) % cycle.size()];
        permutation[destination] = oldPermutation[source];
        orientation[destination] =
            (oldOrientation[source] + orientationDelta[index]) % orientationModulus;
    }
}

void applyClockwiseMove(CubeState& state, Move move) {
    constexpr std::array<int, 4> noCornerTwist{0, 0, 0, 0};
    constexpr std::array<int, 4> noEdgeFlip{0, 0, 0, 0};

    switch (move) {
    case Move::U:
        cyclePieces(state.cornerPerm, state.cornerOrient,
                    {0, 3, 2, 1}, noCornerTwist, 3);
        cyclePieces(state.edgePerm, state.edgeOrient,
                    {0, 3, 2, 1}, noEdgeFlip, 2);
        break;
    case Move::D:
        cyclePieces(state.cornerPerm, state.cornerOrient,
                    {4, 5, 6, 7}, noCornerTwist, 3);
        cyclePieces(state.edgePerm, state.edgeOrient,
                    {4, 5, 6, 7}, noEdgeFlip, 2);
        break;
    case Move::L:
        cyclePieces(state.cornerPerm, state.cornerOrient,
                    {1, 5, 6, 2}, {2, 1, 2, 1}, 3);
        cyclePieces(state.edgePerm, state.edgeOrient,
                    {2, 9, 6, 10}, noEdgeFlip, 2);
        break;
    case Move::R:
        cyclePieces(state.cornerPerm, state.cornerOrient,
                    {0, 3, 7, 4}, {1, 2, 1, 2}, 3);
        cyclePieces(state.edgePerm, state.edgeOrient,
                    {0, 11, 4, 8}, noEdgeFlip, 2);
        break;
    case Move::F:
        cyclePieces(state.cornerPerm, state.cornerOrient,
                    {0, 1, 5, 4}, {1, 2, 1, 2}, 3);
        cyclePieces(state.edgePerm, state.edgeOrient,
                    {1, 9, 5, 8}, {1, 1, 1, 1}, 2);
        break;
    case Move::B:
        cyclePieces(state.cornerPerm, state.cornerOrient,
                    {3, 2, 6, 7}, {2, 1, 2, 1}, 3);
        cyclePieces(state.edgePerm, state.edgeOrient,
                    {3, 10, 7, 11}, {1, 1, 1, 1}, 2);
        break;
    default:
        break;
    }
}

void applyClockwiseMoveRepeatedly(CubeState& state, Move move, int count) {
    for (int application = 0; application < count; ++application) {
        applyClockwiseMove(state, move);
    }
}

}  // namespace

void applyMove(CubeState& state, Move move) {
    // Prime and double moves are derived from clockwise moves so their inverse
    // and repeat behavior cannot drift from the six base move definitions.
    switch (move) {
    case Move::U:
    case Move::D:
    case Move::L:
    case Move::R:
    case Move::F:
    case Move::B:
        applyClockwiseMove(state, move);
        break;
    case Move::UPrime:
        applyClockwiseMoveRepeatedly(state, Move::U, 3);
        break;
    case Move::DPrime:
        applyClockwiseMoveRepeatedly(state, Move::D, 3);
        break;
    case Move::LPrime:
        applyClockwiseMoveRepeatedly(state, Move::L, 3);
        break;
    case Move::RPrime:
        applyClockwiseMoveRepeatedly(state, Move::R, 3);
        break;
    case Move::FPrime:
        applyClockwiseMoveRepeatedly(state, Move::F, 3);
        break;
    case Move::BPrime:
        applyClockwiseMoveRepeatedly(state, Move::B, 3);
        break;
    case Move::U2:
        applyClockwiseMoveRepeatedly(state, Move::U, 2);
        break;
    case Move::D2:
        applyClockwiseMoveRepeatedly(state, Move::D, 2);
        break;
    case Move::L2:
        applyClockwiseMoveRepeatedly(state, Move::L, 2);
        break;
    case Move::R2:
        applyClockwiseMoveRepeatedly(state, Move::R, 2);
        break;
    case Move::F2:
        applyClockwiseMoveRepeatedly(state, Move::F, 2);
        break;
    case Move::B2:
        applyClockwiseMoveRepeatedly(state, Move::B, 2);
        break;
    }
}