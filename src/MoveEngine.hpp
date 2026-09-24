#pragma once

#include "Cube.hpp"

enum class Move {
    U,
    UPrime,
    U2,
    D,
    DPrime,
    D2,
    L,
    LPrime,
    L2,
    R,
    RPrime,
    R2,
    F,
    FPrime,
    F2,
    B,
    BPrime,
    B2
};

void applyMove(CubeState& state, Move move);