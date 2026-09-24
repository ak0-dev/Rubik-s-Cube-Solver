add_executable(move_tests
	test_moves.cpp
	../src/Cube.cpp
	../src/MoveEngine.cpp
)

target_include_directories(move_tests PRIVATE ../src)

add_test(NAME move_cycles COMMAND move_tests)

add_executable(solver_tests
	test_solver.cpp
	../src/Cube.cpp
	../src/MoveEngine.cpp
	../src/Scrambler.cpp
	../src/Heuristic.cpp
	../src/SolverBFS.cpp
	../src/SolverIDAStar.cpp
)

target_include_directories(solver_tests PRIVATE ../src)

add_test(NAME solver_random_scrambles COMMAND solver_tests)
