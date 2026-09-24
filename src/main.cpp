#include <iostream>
#include <chrono>

#include "Benchmark.hpp"
#include "Scrambler.hpp"
#include "SolverBFS.hpp"
#include "SolverIDAStar.hpp"

namespace {

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

const char* statusName(SolveStatus status) {
    switch (status) {
    case SolveStatus::Solved: return "Solved";
    case SolveStatus::NotFound: return "Not found";
    case SolveStatus::Timeout: return "Timeout";
    }

    return "Unknown";
}

void printMoves(const std::vector<Move>& moves) {
    if (moves.empty()) {
        std::cout << "(empty)";
        return;
    }

    for (std::size_t index = 0; index < moves.size(); ++index) {
        if (index > 0) {
            std::cout << ' ';
        }
        std::cout << moveName(moves[index]);
    }
}

void applyMoves(CubeState& state, const std::vector<Move>& moves) {
    for (Move move : moves) {
        applyMove(state, move);
    }
}

template <typename Array>
void printArray(const char* name, const Array& values) {
    std::cout << name << ": [";
    for (std::size_t index = 0; index < values.size(); ++index) {
        if (index > 0) {
            std::cout << ", ";
        }
        std::cout << values[index];
    }
    std::cout << "]\n";
}

void printMenu() {
    std::cout << "\n1. Scramble cube  2. Solve with BFS (baseline)  "
                 "3. Solve with IDA*  4. Run benchmark suite  "
                 "5. Display current cube state  6. Exit\n";
    std::cout << "Choice: ";
}

}  // namespace

int main() {
    std::cout << "Rubik's Cube Solver\n";
    CubeState state;
    bool running = true;

    while (running) {
        printMenu();

        int choice = 0;
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "Please enter a number from 1 to 6.\n";
            continue;
        }

        switch (choice) {
        case 1: {
            int moveCount = 0;
            std::cout << "Number of scramble moves: ";
            if (!(std::cin >> moveCount)) {
                std::cin.clear();
                std::cin.ignore(10000, '\n');
                std::cout << "Invalid move count.\n";
                break;
            }
            if (moveCount < 0) {
                std::cout << "Move count cannot be negative.\n";
                break;
            }

            const std::vector<Move> moves = scramble(state, moveCount);
            std::cout << "Moves applied: ";
            printMoves(moves);
            std::cout << "\n";
            break;
        }
        case 2: {
            const auto startTime = std::chrono::steady_clock::now();
            const SolveResult result = solveBFS(state, 9, std::chrono::seconds(10));
            const auto endTime = std::chrono::steady_clock::now();
            const std::chrono::duration<double, std::milli> elapsed = endTime - startTime;

            std::cout << "Status: " << statusName(result.status) << "\n";
            std::cout << "Solution: ";
            printMoves(result.moves);
            std::cout << "\nNodes explored: " << result.statesExplored
                      << "\nTime (ms): " << elapsed.count() << "\n";
            if (result.status == SolveStatus::Solved) {
                applyMoves(state, result.moves);
                std::cout << std::boolalpha
                          << "Cube solved after applying solution: "
                          << state.isSolved() << "\n";
            }
            break;
        }
        case 3: {
            const auto startTime = std::chrono::steady_clock::now();
            const IDAStarResult result = solveIDAStar(state, 30, std::chrono::seconds(10));
            const auto endTime = std::chrono::steady_clock::now();
            const std::chrono::duration<double, std::milli> elapsed = endTime - startTime;

            std::cout << "Status: "
                      << (result.found ? "Solved" :
                          (result.timedOut ? "Timeout" : "Not found")) << "\n";
            std::cout << "Solution: ";
            printMoves(result.moves);
            std::cout << "\nNodes explored: " << result.nodesExplored
                      << "\nTime (ms): " << elapsed.count() << "\n";
            if (result.found) {
                applyMoves(state, result.moves);
                std::cout << std::boolalpha
                          << "Cube solved after applying solution: "
                          << state.isSolved() << "\n";
            }
            break;
        }
        case 4:
            runBenchmark();
            break;
        case 5:
            printArray("cornerPerm", state.cornerPerm);
            printArray("cornerOrient", state.cornerOrient);
            printArray("edgePerm", state.edgePerm);
            printArray("edgeOrient", state.edgeOrient);
            break;
        case 6:
            running = false;
            break;
        default:
            std::cout << "Please choose an option from 1 to 6.\n";
            break;
        }
    }

    std::cout << "Goodbye.\n";
    return 0;
}
