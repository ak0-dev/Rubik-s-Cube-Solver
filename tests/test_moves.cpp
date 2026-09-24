#include "Cube.hpp"
#include "MoveEngine.hpp"

#include <array>
#include <iostream>

int main() {
    const std::array<std::pair<const char*, Move>, 6> baseMoves{
        std::pair{"U", Move::U},
        std::pair{"D", Move::D},
        std::pair{"L", Move::L},
        std::pair{"R", Move::R},
        std::pair{"F", Move::F},
        std::pair{"B", Move::B},
    };

    bool allPassed = true;

    for (const auto& [name, move] : baseMoves) {
        CubeState cube;
        for (int turn = 0; turn < 4; ++turn) {
            applyMove(cube, move);
        }

        const bool passed = cube.isSolved();
        std::cout << name << ": " << (passed ? "PASS" : "FAIL") << '\n';
        allPassed = allPassed && passed;
    }

    const std::array<Move, 4> triggerSequence{
        Move::R, Move::U, Move::RPrime, Move::UPrime};
    CubeState triggerCube;
    for (int repetition = 0; repetition < 6; ++repetition) {
        for (Move move : triggerSequence) {
            applyMove(triggerCube, move);
        }
    }

    const bool triggerPassed = triggerCube.isSolved();
    std::cout << "R U R' U' x6: "
              << (triggerPassed ? "PASS" : "FAIL") << '\n';
    allPassed = allPassed && triggerPassed;

    return allPassed ? 0 : 1;
}