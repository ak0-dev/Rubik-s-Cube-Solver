#include "Cube.hpp"

CubeState::CubeState()
    : cornerPerm{0, 1, 2, 3, 4, 5, 6, 7},
      cornerOrient{},
      edgePerm{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11},
      edgeOrient{} {
}

bool CubeState::isSolved() const {
    return *this == CubeState{};
}

bool CubeState::operator==(const CubeState& other) const {
    return cornerPerm == other.cornerPerm &&
           cornerOrient == other.cornerOrient &&
           edgePerm == other.edgePerm &&
           edgeOrient == other.edgeOrient;
}