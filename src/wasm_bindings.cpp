#include <emscripten/bind.h>

#include "Scrambler.hpp"
#include "SolverBFS.hpp"
#include "SolverIDAStar.hpp"

#include <chrono>
#include <cstddef>
#include <string>
#include <vector>

namespace {

CubeState currentCube;

const char* moveName(Move move) {
    switch (move) {
    case Move::U: return "U";
    case Move::UPrime: return "U'";
    case Move::U2: return "U2";
    case Move::D: return "D";
    case Move::DPrime: return "D'";
    case Move::D2: return "D2";
    case Move::L: return "L";
    case Move::LPrime: return "L'";
    case Move::L2: return "L2";
    case Move::R: return "R";
    case Move::RPrime: return "R'";
    case Move::R2: return "R2";
    case Move::F: return "F";
    case Move::FPrime: return "F'";
    case Move::F2: return "F2";
    case Move::B: return "B";
    case Move::BPrime: return "B'";
    case Move::B2: return "B2";
    }

    return "?";
}

std::vector<std::string> readableMoves(const std::vector<Move>& moves) {
    std::vector<std::string> result;
    result.reserve(moves.size());
    for (Move move : moves) {
        result.emplace_back(moveName(move));
    }
    return result;
}

const char* statusName(SolveStatus status) {
    switch (status) {
    case SolveStatus::Solved: return "Solved";
    case SolveStatus::NotFound: return "Not found";
    case SolveStatus::Timeout: return "Timeout";
    }

    return "Unknown";
}

struct MoveList {
    std::vector<std::string> moves;
};

struct WebSolveResult {
    std::string status;
    std::vector<std::string> moves;
    std::size_t nodesExplored;
};

void resetCube() {
    currentCube = CubeState{};
}

MoveList scrambleCube(int numMoves) {
    if (numMoves < 0) {
        numMoves = 0;
    }

    return {readableMoves(scramble(currentCube, numMoves))};
}

WebSolveResult solveCurrentCubeBFSWithOptions(int maxDepth, int timeoutMs) {
    const SolveResult result = solveBFS(currentCube,
                                        maxDepth,
                                        std::chrono::milliseconds(timeoutMs));
    return {statusName(result.status),
            readableMoves(result.moves),
            result.statesExplored};
}

WebSolveResult solveCurrentCubeBFS() {
    return solveCurrentCubeBFSWithOptions(9, 10000);
}

WebSolveResult solveCurrentCubeIDAStarWithOptions(int maxDepth, int timeoutMs) {
    const IDAStarResult result = solveIDAStar(
        currentCube, maxDepth, std::chrono::milliseconds(timeoutMs));
    const char* status = result.found
                             ? "Solved"
                             : (result.timedOut ? "Timeout" : "Not found");
    return {status, readableMoves(result.moves), result.nodesExplored};
}

WebSolveResult solveCurrentCubeIDAStar() {
    return solveCurrentCubeIDAStarWithOptions(30, 10000);
}

}  // namespace

EMSCRIPTEN_BINDINGS(cube_solver) {
    emscripten::value_object<MoveList>("MoveList")
        .field("moves", &MoveList::moves);

    emscripten::value_object<WebSolveResult>("SolveResult")
        .field("status", &WebSolveResult::status)
        .field("moves", &WebSolveResult::moves)
        .field("nodesExplored", &WebSolveResult::nodesExplored);

    emscripten::register_vector<std::string>("StringVector");

    emscripten::function("resetCube", &resetCube);
    emscripten::function("scrambleCube", &scrambleCube);
    emscripten::function("solveCurrentCubeBFS", &solveCurrentCubeBFS);
    emscripten::function("solveCurrentCubeBFSWithOptions",
                         &solveCurrentCubeBFSWithOptions);
    emscripten::function("solveCurrentCubeIDAStar", &solveCurrentCubeIDAStar);
    emscripten::function("solveCurrentCubeIDAStarWithOptions",
                         &solveCurrentCubeIDAStarWithOptions);
}