#include "Heuristic.hpp"

#include <algorithm>

int heuristic(const CubeState& state) {
    int misplacedCorners = 0;
    for (std::size_t position = 0; position < state.cornerPerm.size(); ++position) {
        if (state.cornerPerm[position] != static_cast<int>(position) ||
            state.cornerOrient[position] != 0) {
            ++misplacedCorners;
        }
    }

    int misplacedEdges = 0;
    for (std::size_t position = 0; position < state.edgePerm.size(); ++position) {
        if (state.edgePerm[position] != static_cast<int>(position) ||
            state.edgeOrient[position] != 0) {
            ++misplacedEdges;
        }
    }

    const int cornerEstimate = (misplacedCorners + 3) / 4;
    const int edgeEstimate = (misplacedEdges + 3) / 4;

    // Each component is admissible, and max preserves admissibility because a
    // single move can improve both components at once; summing could overestimate.
    return std::max(cornerEstimate, edgeEstimate);
}