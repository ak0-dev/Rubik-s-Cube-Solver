#pragma once

#include <array>

class CubeState {
public:
    // A permutation records which piece occupies each position.
    // An orientation records how that piece is twisted or flipped in place.
    std::array<int, 8> cornerPerm;
    std::array<int, 8> cornerOrient;
    std::array<int, 12> edgePerm;
    std::array<int, 12> edgeOrient;

    CubeState();

    bool isSolved() const;
    bool operator==(const CubeState& other) const;
};