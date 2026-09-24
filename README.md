# Rubik's Cube Solver

This project is a C++17 Rubik's Cube solver built with CMake. It models cube states, applies standard face moves, generates scrambles, and searches for solutions with breadth-first search (BFS) and heuristic-guided Iterative Deepening A* (IDA*).

The motivation is to make the structure of a cube solver explicit: represent the state compactly, treat moves as graph edges, compare uninformed and informed search, and measure how their behavior changes as scrambles become deeper.

## Graph Framing

Each reachable cube state is a node in an implicit graph. Each legal move is an edge from one state to another. Solving is therefore a shortest-path search from the scrambled node to the solved node.

The full state space contains approximately 4.3 x 10^19 reachable states. The project never constructs that graph in memory. BFS, IDA*, and the scrambler generate successor states on the fly as they need them.

## Cube Representation

The solver uses a cubie model rather than a facelet array:

- `cornerPerm[8]` identifies which corner cubie occupies each corner position.
- `cornerOrient[8]` stores the twist of each corner, with values from 0 to 2.
- `edgePerm[12]` identifies which edge cubie occupies each edge position.
- `edgeOrient[12]` stores the flip of each edge, with values from 0 to 1.

A facelet model would store the color of every visible sticker, typically as 54 face values. That representation is intuitive for rendering and camera input, but moves must update many stickers and validity checks are more involved. The cubie model was chosen because it is compact, directly represents the pieces moved by each turn, and makes permutation/orientation updates and search-state comparisons efficient.

## Solvers

### Breadth-First Search

BFS expands states by increasing move distance from the scramble. When it finds the solved state, the returned sequence is a shortest solution under the move set. Its weakness is memory: the number of states grows exponentially with depth, so storing the frontier and visited set quickly becomes impractical. This is the state-space explosion that limits BFS to relatively shallow scrambles here.

### IDA*

IDA* performs depth-first searches with increasing thresholds on:

```text
f(state) = g(state) + h(state)
```

`g` is the number of moves already taken, and `h` is the heuristic estimate. The search uses a maximum depth or timeout where applicable so deeper searches fail gracefully.

The heuristic counts corners that are in the wrong position or orientation and rounds that count divided by 4 upward. It does the same for edges, then returns the larger of the two values:

```text
cornerEstimate = ceil(misplacedCorners / 4)
edgeEstimate   = ceil(misplacedEdges / 4)
heuristic      = max(cornerEstimate, edgeEstimate)
```

This is admissible because each estimate is a lower bound on the moves still required: one move can affect at most four corners or four edges. Taking the maximum of admissible lower bounds remains a lower bound on the true solution distance. Summing them is not safe because one move can improve both the corner and edge measures at the same time, causing the sum to overestimate.

Both solvers prune a move that directly undoes the previous move. IDA* also prunes three consecutive turns of the same face because that sequence can be represented by an equivalent one- or two-turn move.

## Benchmark Results

The benchmark uses the same randomly generated scramble for BFS and IDA* at each listed depth. Each solver has a two-second timeout. Times are from the recorded `benchmark_results.csv` run.

| Scramble depth | Solver | Status | Time (ms) | Nodes explored | Solution length |
|---:|:---|:---|---:|---:|---:|
| 3 | BFS | success | 9.339 | 239 | 3 |
| 3 | IDA* | success | 0.100 | 90 | 3 |
| 5 | BFS | success | 670.431 | 32,850 | 5 |
| 5 | IDA* | success | 2.116 | 5,538 | 5 |
| 7 | BFS | timeout | 2,355.678 | 108,924 | - |
| 7 | IDA* | success | 206.464 | 572,156 | 7 |
| 9 | BFS | timeout | 2,308.052 | 109,272 | - |
| 9 | IDA* | timeout | 2,000.002 | 5,120,794 | - |

The results show the expected tradeoff. BFS is straightforward and guarantees a shortest solution when it finishes, but its explored-state and memory costs rise sharply. IDA* explores more nodes than BFS in some successful cases, but the heuristic lets it use much less memory and solve the depth-7 sample where BFS timed out. At depth 9, even IDA* reaches the configured timeout, showing that the basic heuristic is useful but not sufficient for difficult full-size scrambles.

## Known Limitations

BFS currently tracks visited cube states without including the previous move in the visited key, while inverse-move pruning depends on that previous move. The same cube state can therefore be reached through histories with different legal next moves. During testing, a history-aware visited key was explored; it improved completeness of the pruning model but substantially increased the BFS search cost and memory pressure for this implementation.

That interaction was scoped out rather than fixed because this project is focused on comparing the baseline BFS and IDA* implementations, and the change would require a broader BFS state-management redesign. The limitation is one reason BFS is treated as a baseline and is bounded by a timeout.

## Future Improvements

- **Pattern databases:** Build reusable admissible lookup tables with backward BFS from the solved state. Pattern databases would provide much stronger heuristics than the current misplaced-piece count, at the cost of preprocessing and memory.
- **Camera-based scanning:** Add a camera input pipeline that recognizes sticker colors and constructs a `CubeState`. This is out of scope because it requires image capture, color calibration, face detection, and robust cube-pose handling.

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
