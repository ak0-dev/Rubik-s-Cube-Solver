# Rubik's Cube Solver

This project is a C++17 Rubik's Cube solver built with CMake and Emscripten. It models cube states, applies standard face moves, generates scrambles, and searches for solutions with breadth-first search (BFS) and heuristic-guided Iterative Deepening A* (IDA*). The same solver core is available as a native CLI and as a browser application backed by WebAssembly.

The project compares an uninformed shortest-path search with a heuristic-guided search in both native and browser environments.

## Repository Layout

- `src/`: native solver, cube model, move engine, scrambler, heuristics, and WebAssembly bindings.
- `tests/`: native move and solver tests.
- `web/index.html`: browser UI with the Three.js cube and solver controls.
- `web/cube_solver.js` and `web/cube_solver.wasm`: browser-ready Emscripten artifacts.
- `build/`: native CMake output and local WebAssembly build output.

## Solvers

### Breadth-First Search

BFS expands states by increasing move distance from the scramble. When it finds the solved state, the returned sequence is a shortest solution under the move set. Its weakness is memory: the number of states grows exponentially with depth, so storing the frontier and visited set quickly becomes impractical. This is the state-space explosion that limits BFS to relatively shallow scrambles here.

### IDA*

IDA* performs depth-first searches with increasing thresholds on:

```text
f(state) = g(state) + h(state)
```

`g` is the number of moves already taken, and `h` is the heuristic estimate. The search uses a maximum depth or timeout where applicable so deeper searches fail gracefully.

The heuristic counts misplaced or incorrectly oriented corners and edges, divides each count by four, rounds upward, and uses the larger estimate. This is admissible because one face turn affects at most four corners or four edges.

```text
heuristic = max(ceil(misplacedCorners / 4), ceil(misplacedEdges / 4))
```

Both solvers prune a move that directly undoes the previous move. IDA* also prunes three consecutive turns of the same face because that sequence can be represented by an equivalent one- or two-turn move.

## Browser Application

The browser app provides:

- A 3D solved-looking cube with 26 cubies rendered by Three.js.
- OrbitControls for mouse/touch rotation and scroll or pinch zoom.
- A scramble count input and Scramble, Solve with BFS, and Solve with IDA* buttons.
- Step-by-step solution playback with a 500 ms delay between moves.
- Results for status, moves, explored nodes, and elapsed time.
- A solved-state check that skips search and animation when the cube is already solved.

The WASM cube is the authoritative state. Scramble moves and solution moves update both the WASM state and the visual cube, so a second solve correctly reports `Cube is already solved`.

### WebAssembly Build

Install and activate Emscripten in a sibling directory such as `F:\Coding\emsdk`, then run this command from the project root:

```powershell
emcc -std=c++17 -O3 --bind -fexceptions `
	-sDISABLE_EXCEPTION_CATCHING=0 `
	-sDEFAULT_TO_CXX=1 `
	-sALLOW_MEMORY_GROWTH=1 `
	-sINITIAL_MEMORY=33554432 `
	-sASSERTIONS=1 `
	src/wasm_bindings.cpp `
	src/Cube.cpp src/MoveEngine.cpp src/Scrambler.cpp `
	src/Heuristic.cpp src/SolverBFS.cpp src/SolverIDAStar.cpp `
	-o build/cube_solver.js `
	-sMODULARIZE=1 -sEXPORT_ES6=1
```

For deployment, copy the generated files into `web/` because `web` is the static hosting output directory:

```powershell
Copy-Item build\cube_solver.js web\cube_solver.js -Force
Copy-Item build\cube_solver.wasm web\cube_solver.wasm -Force
```

`web/index.html` imports the module with `./cube_solver.js`.

### Run Locally

Browsers must load the JavaScript module and WASM file over HTTP. From the project root:

```powershell
python -m http.server 8000 -d web
```

Open [http://localhost:8000/](http://localhost:8000/).

### Deploy with Vercel

Push the repository to GitHub, import it into Vercel, and configure it as a static project:

- Framework preset: `Other`
- Build command: empty
- Install command: empty
- Output directory: `web`

The committed `web/cube_solver.js` and `web/cube_solver.wasm` files make the output directory self-contained.

## WebAssembly Safety Limits

The browser build uses a 32 MB initial heap with dynamic growth:

```text
-sINITIAL_MEMORY=33554432
-sALLOW_MEMORY_GROWTH=1
```

The practical browser limit is approximately 2 GB for this wasm32 build. BFS is memory-heavy because it stores a queue and visited states, so the browser binding uses:

- Maximum search depth: 10 moves.
- Wall-clock timeout: 60 seconds.
- Visited-state cap: 5,000,000 states.

When BFS reaches its state cap, it returns `Timeout` instead of continuing toward a hard allocation failure. Diagnostic result fields also expose the stored-state count and stop reason during development.

IDA* searches depth-first and does not maintain a large visited-state set. Its browser limits are:

- Maximum search depth: 30 moves.
- Wall-clock timeout: 60 seconds.
- No arbitrary node-count cap.

Both bindings catch C++ allocation/runtime exceptions where Emscripten can deliver them. The assertions build is useful for diagnostics, but a hard WebAssembly memory-growth abort can occur before C++ catches it, which is why BFS has a proactive state cap.

## Known Limitations

BFS currently tracks visited cube states without including the previous move in the visited key, while inverse-move pruning depends on that previous move. The same cube state can therefore be reached through histories with different legal next moves. During testing, a history-aware visited key was explored; it improved completeness of the pruning model but substantially increased the BFS search cost and memory pressure for this implementation.

This is a known tradeoff of the baseline BFS implementation and contributes to its memory and performance limits.

The browser search runs synchronously on the main thread. A long BFS or IDA* search can therefore make the page unresponsive until the solver returns. A Web Worker is the natural next step for interruptible, non-blocking searches.

## Build and Run

From the project root in PowerShell:

```powershell
cmake -S . -B build
cmake --build build
```

Run the interactive solver:

```powershell
.\build\cube_solver.exe
```

The menu supports scrambling the persistent cube, solving it with BFS or IDA*, displaying the cubie arrays, and running the benchmark suite. The benchmark writes `benchmark_results.csv` in the current working directory.

Run all tests:

```powershell
ctest --test-dir build --output-on-failure
```

The test executables can also be run directly:

```powershell
.\build\tests\move_tests.exe
.\build\tests\solver_tests.exe
```

The native CMake build does not include `src/wasm_bindings.cpp`; the WebAssembly module is a separate entry point and is built with the `emcc` command above.
