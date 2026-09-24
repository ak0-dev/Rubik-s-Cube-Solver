import createCubeSolver from "./cube_solver.js";

let modulePromise = createCubeSolver();
let module;

function moveStrings(moveVector) {
  return Array.from(
    { length: moveVector.size() },
    (_, index) => moveVector.get(index),
  );
}

function applyMoves(moves) {
  for (const move of moves) {
    module.applyMoveToCurrentCube(move);
  }
}

async function handleMessage(event) {
  module = module || await modulePromise;
  const { type, requestId } = event.data;

  if (type === "scramble") {
    module.resetCube();
    const scramble = moveStrings(module.scrambleCube(event.data.count).moves);
    self.postMessage({
      type: "scrambleResult",
      requestId,
      moves: scramble,
      solved: module.isCubeSolved(),
    });
    return;
  }

  if (type === "restore") {
    module.resetCube();
    applyMoves(event.data.moves);
    self.postMessage({ type: "restored", requestId });
    return;
  }

  if (type === "solve") {
    const start = performance.now();
    const result = event.data.algorithm === "BFS"
      ? module.solveCurrentCubeBFS()
      : module.solveCurrentCubeIDAStar();
    const moves = moveStrings(result.moves);

    if (result.status === "Solved") {
      applyMoves(moves);
    }

    self.postMessage({
      type: "solveResult",
      requestId,
      algorithm: event.data.algorithm,
      status: result.status,
      moves,
      nodesExplored: result.nodesExplored,
      elapsedMs: performance.now() - start,
      solved: module.isCubeSolved(),
    });
  }
}

modulePromise
  .then(() => self.postMessage({ type: "ready" }))
  .catch(error => self.postMessage({ type: "error", message: String(error) }));

self.onmessage = event => {
  handleMessage(event).catch(error => {
    self.postMessage({
      type: "error",
      requestId: event.data.requestId,
      message: String(error),
    });
  });
};
