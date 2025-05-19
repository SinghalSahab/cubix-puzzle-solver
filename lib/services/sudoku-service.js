let sudokuModule = null;
let sudokuEngine = null;
const BASE_PATH = process.env.NODE_ENV === 'production' ? '/cubix' : '';

async function loadSudokuModule() {
  if (sudokuModule !== null) {
    return sudokuModule;
  }

  try {
    if (typeof window === "undefined")
      throw new Error("WASM modules can only be loaded in the browser");

    const wasm = await import(/* webpackIgnore: true */ `${BASE_PATH}/wasm/sudoku.js`);
    const factory = wasm.default;
    sudokuModule = await factory();
    return sudokuModule;
  } catch (error) {
    console.error("Failed to load Sudoku WASM module:", error);
    throw new Error("Failed to initialize Sudoku engine");
  }
}

export const SudokuService = {
  initSolver: async () => {
    try {
      const module = await loadSudokuModule();
      sudokuEngine = new module.SudokuEngine();
      return { success: true };
    } catch (error) {
      console.error("Initialization error:", error);
      throw error;
    }
  },

  solvePuzzle: async (puzzle) => {
    try {
      if (!sudokuEngine) {
        await SudokuService.initSolver();
      }
      const flatPuzzle = Array.isArray(puzzle[0]) ? puzzle.flat().map(Number) : puzzle.map(Number);
      const vec = new sudokuModule.IntVector();
      flatPuzzle.forEach(n => vec.push_back(n));
      const rawResult = sudokuEngine.solve(vec);
      vec.delete(); // Important to free the memory
      const result = typeof rawResult === 'string' ? rawResult : String(rawResult);

      if (result.startsWith("Error:")) {
        throw new Error(result);
      }
      return { output: result }; 
    } catch (error) {
      console.error("Solver error:", error);
      throw error;
    }
  },

  checkPuzzle: async (puzzle) => {
    try {
      if (!sudokuEngine) {
        await SudokuService.initSolver();
      }
      const flatPuzzle = Array.isArray(puzzle[0]) ? puzzle.flat().map(Number) : puzzle.map(Number);
      const vec = new sudokuModule.IntVector();
      flatPuzzle.forEach(n => vec.push_back(n));
      const rawResult = sudokuEngine.checkValidity(vec);
      vec.delete();
      const result = typeof rawResult === 'string' ? rawResult : String(rawResult);
      
      return { output: result.split(':')[0] }; 
    } catch (error) {
      console.error("Validity check error:", error);
      throw error;
    }
  },

  getHint: async (puzzle, row, col) => {
    try {
      if (!sudokuEngine) {
        await SudokuService.initSolver();
      }
      const flatPuzzle = Array.isArray(puzzle[0]) ? puzzle.flat().map(Number) : puzzle.map(Number);
      const vec = new sudokuModule.IntVector();
      flatPuzzle.forEach(n => vec.push_back(n));
      const rawResult = sudokuEngine.getHint(vec, row, col);
      vec.delete();
      const result = typeof rawResult === 'string' ? rawResult : String(rawResult);

      if (result.startsWith("Error:")) {
        throw new Error(result);
      }
      return { output: result };
    } catch (error) {
      console.error("Hint error:", error);
      throw error;
    }
  },
  compileSolver: async () => await SudokuService.initSolver(),
};
