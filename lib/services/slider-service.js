let sliderModule = null;
let sliderSolver = null;

const BASE_PATH = process.env.NODE_ENV === "production" ? "/cubix" : "";

async function loadSliderModule() {
  if (sliderModule !== null) {
    return sliderModule;
  }

  try {
    if (typeof window === "undefined") {
      throw new Error("WASM modules can only be loaded in the browser");
    }

    const wasm = await import(
      /* webpackIgnore: true */ `${BASE_PATH}/wasm/number_slider.js`
    );
    const factory = wasm.default;
    sliderModule = await factory();
    return sliderModule;
  } catch (error) {
    console.error("Failed to load Number Slider WASM module:", error);
    throw new Error("Failed to initialize Number Slider engine");
  }
}

export const SliderService = {
  initSolver: async () => {
    try {
      const module = await loadSliderModule();
      sliderSolver = new module.NumberSliderEngine();
      return { success: true };
    } catch (error) {
      console.error("Initialization error:", error);
      throw error;
    }
  },

  solvePuzzle: async (puzzle) => {
    try {
      if (!sliderSolver) {
        await SliderService.initSolver();
      }
      const flatPuzzle = Array.isArray(puzzle[0]) ? puzzle.flat() : puzzle;
      const vec = new sliderModule.IntVector();
      flatPuzzle.forEach((n) => vec.push_back(n));
      const result = sliderSolver.solve(vec);
      console.log(result);
      vec.delete();

      if (result.startsWith("Error:")) {
        throw new Error(result);
      }
      const trimmed = result.trim();
      let moves;
      const dirs = trimmed.match(/[UDLR]/g);
      if (dirs && dirs.length === trimmed.length) {
        moves = dirs;
      } else {
        moves = trimmed.split(" ").filter((m) => m.length > 0);
      }

      return {
        moves,
        totalMoves: moves.length,
      };
    } catch (error) {
      console.error("Solver error:", error);
      throw error;
    }
  },
  compileSolver: async () => await SliderService.initSolver(),
};
