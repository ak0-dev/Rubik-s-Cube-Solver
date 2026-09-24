cmake_minimum_required(VERSION 3.16)

project(cube_solver LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(cube_solver
	src/main.cpp
	src/Cube.cpp
	src/MoveEngine.cpp
	src/Scrambler.cpp
	src/SolverBFS.cpp
	src/Heuristic.cpp
	src/SolverIDAStar.cpp
	src/Benchmark.cpp
)

enable_testing()
add_subdirectory(tests)
