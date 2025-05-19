let rubixModule = null;
let rubixSolver = null;

const BASE_PATH = process.env.NODE_ENV === "production" ? "/cubix" : "";

async function loadRubixModule() {
  console.log("Starting to load Rubix module");
  if (rubixModule !== null) {
    console.log("Rubix module already loaded, reusing instance");
    return rubixModule;
  }

  try {
    if (typeof window === "undefined") {
      throw new Error("WASM modules can only be loaded in the browser");
    }

    console.log(`Loading WASM from path: ${BASE_PATH}/wasm/rubix.js`);
    const wasm = await import(
      /* webpackIgnore: true */ `${BASE_PATH}/wasm/rubix.js`
    );
    const factory = wasm.default;
    console.log("Factory obtained, instantiating module");
    rubixModule = await factory();
    console.log("Rubix module loaded successfully");
    return rubixModule;
  } catch (error) {
    console.error("Failed to load Rubik's Cube WASM module:", error);
    throw new Error("Failed to initialize Rubik's Cube engine");
  }
}

export const RubixService = {
  initSolver: async () => {
    console.log("Initializing Rubix solver");
    try {
      const module = await loadRubixModule();
      console.log("Creating RubiksCubeEngine instance");
      rubixSolver = new module.RubiksCubeEngine();
      console.log("Solver initialized successfully");
      return { success: true };
    } catch (error) {
      console.error("Initialization error:", error);
      throw error;
    }
  },

  solvePuzzle: async (startState, finalState) => {
    console.log("Starting to solve puzzle");
    try {
      if (!rubixSolver) {
        console.log("Solver not initialized, initializing now");
        await RubixService.initSolver();
      }

      console.log("Preparing start cube state for solver");
      const jsFlatStartState = startState.flat().flat().map(Number);
      console.log("Flattened start state:", jsFlatStartState);

      const vecStartState = new rubixModule.IntVector();
      jsFlatStartState.forEach(n => vecStartState.push_back(n));
      console.log("Start state vector created");

      console.log("Setting start state");
      rubixSolver.setStartState(vecStartState);
      vecStartState.delete();

      if (finalState) {
        console.log("Preparing final cube state for solver");
        const jsFlatFinalState = finalState.flat().flat().map(Number);
        console.log("Flattened final state:", jsFlatFinalState);

        const vecFinalState = new rubixModule.IntVector();
        jsFlatFinalState.forEach(n => vecFinalState.push_back(n));
        console.log("Final state vector created");

        console.log("Setting final state");
        rubixSolver.setGoalState(vecFinalState);
        vecFinalState.delete();
      }

      console.log("Executing solve algorithm");
      const result = rubixSolver.solve();

      if (result.startsWith("Error:") || result === "Already solved or no solution found." || result === "No solution found (exhausted search space or no solution within limits)." || result === "Search limit exceeded (cost too high). Might be unsolvable or too complex." || result === "Error: Solver loop iterations exceeded.") {
        console.error("Solver returned message:", result);
        if (result.startsWith("Error:")) {
          throw new Error(result);
        }
        return {
          output: result,
          moves: [],
          totalMoves: 0,
        };
      }

      const moves = result
        .trim()
        .split(" ")
        .filter((move) => move.length > 0);

      console.log("Solve complete, found", moves.length, "moves");
      return {
        output: result,
        moves,
        totalMoves: moves.length,
      };
    } catch (error) {
      console.error("Solver error:", error);
      throw error;
    }
  },

  isSolved: async (cubeState) => {
    console.log("Checking if cube is solved");
    try {
      if (!rubixSolver) {
        console.log("Solver not initialized, initializing now");
        await RubixService.initSolver();
      }

      const jsFlatState = cubeState.flat().flat().map(Number);
      console.log("Flattened state for check:", jsFlatState);
      
      const vecCubeState = new rubixModule.IntVector();
      jsFlatState.forEach(n => vecCubeState.push_back(n));

      console.log("Calling isSolved on the WASM module");
      const result = rubixSolver.isSolved(vecCubeState);
      vecCubeState.delete();

      console.log("Solved check result:", result);
      return {
        solved: result === "true",
      };
    } catch (error) {
      console.error("Check error:", error);
      throw error;
    }
  },

  compileSolver: async () => {
    console.log("Compiling solver (alias for initSolver)");
    return await RubixService.initSolver();
  },
};
