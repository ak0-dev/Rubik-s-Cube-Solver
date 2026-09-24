#pragma once

#include "Cube.hpp"
#include "MoveEngine.hpp"

#include <vector>

std::vector<Move> scramble(CubeState& state, int numMoves);