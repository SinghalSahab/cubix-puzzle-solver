"use client";
import { useState, useEffect } from "react";
import { Button } from "@/components/ui/button";
import { toast } from "@/components/ui/use-toast";
import { RubixService } from "@/lib/services/rubix-service";
import RubiksCube3D from "./RubiksCube3D";

const COLORS = {
  0: {
    name: "White",
    bg: "bg-white",
    border: "border-gray-300",
    text: "text-black",
  },
  1: {
    name: "Orange",
    bg: "bg-orange-500",
    border: "border-orange-700",
    text: "text-white",
  },
  2: {
    name: "Green",
    bg: "bg-green-500",
    border: "border-green-700",
    text: "text-white",
  },
  3: {
    name: "Red",
    bg: "bg-red-500",
    border: "border-red-700",
    text: "text-white",
  },
  4: {
    name: "Blue",
    bg: "bg-blue-500",
    border: "border-blue-700",
    text: "text-white",
  },
  5: {
    name: "Yellow",
    bg: "bg-yellow-400",
    border: "border-yellow-600",
    text: "text-black",
  },
};

const MOVE_DESCRIPTIONS = {
  U: "Rotate Up face clockwise",
  "U'": "Rotate Up face anti-clockwise",
  U2: "Rotate Up face 180 degrees",
  D: "Rotate Down face clockwise",
  "D'": "Rotate Down face anti-clockwise",
  D2: "Rotate Down face 180 degrees",
  L: "Rotate Left face clockwise",
  "L'": "Rotate Left face anti-clockwise",
  L2: "Rotate Left face 180 degrees",
  R: "Rotate Right face clockwise",
  "R'": "Rotate Right face anti-clockwise",
  R2: "Rotate Right face 180 degrees",
  F: "Rotate Front face clockwise",
  "F'": "Rotate Front face anti-clockwise",
  F2: "Rotate Front face 180 degrees",
  B: "Rotate Back face clockwise",
  "B'": "Rotate Back face anti-clockwise",
  B2: "Rotate Back face 180 degrees",
};

const cloneCubeState = (state) => JSON.parse(JSON.stringify(state));
const applyMove = (currentCubeState, move) => {
  console.log(`applyMove (2D placeholder) called with: ${move}`);
  return currentCubeState;
};

export default function RubiksCube2D() {
  const [startState, setStartState] = useState([
    [
      [0, 0, 0],
      [0, 5, 5],
      [0, 3, 4],
    ],
    [
      [1, 1, 1],
      [3, 1, 2],
      [0, 1, 2],
    ],
    [
      [2, 2, 2],
      [4, 2, 0],
      [1, 2, 3],
    ],
    [
      [3, 3, 3],
      [2, 3, 5],
      [4, 3, 1],
    ],
    [
      [4, 4, 4],
      [1, 4, 0],
      [3, 4, 5],
    ],
    [
      [5, 5, 5],
      [5, 0, 1],
      [5, 2, 4],
    ],
  ]);

  const [finalState, setFinalState] = useState([
    [
      [0, 0, 0],
      [0, 0, 0],
      [0, 0, 0],
    ],
    [
      [1, 1, 1],
      [1, 1, 1],
      [1, 1, 1],
    ],
    [
      [2, 2, 2],
      [2, 2, 2],
      [2, 2, 2],
    ],
    [
      [3, 3, 3],
      [3, 3, 3],
      [3, 3, 3],
    ],
    [
      [4, 4, 4],
      [4, 4, 4],
      [4, 4, 4],
    ],
    [
      [5, 5, 5],
      [5, 5, 5],
      [5, 5, 5],
    ],
  ]);

  const [solutionStepsForDisplay, setSolutionStepsForDisplay] = useState([]);
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState(null);

  const [simulationViewActive, setSimulationViewActive] = useState(false);
  const [simulationInitialState, setSimulationInitialState] = useState(null);
  const [simulationSolutionMoves, setSimulationSolutionMoves] = useState([]);
  const [trigger3DMoveCounter, setTrigger3DMoveCounter] = useState(0);
  const [
    currentDisplayedSolutionStepIndex,
    setCurrentDisplayedSolutionStepIndex,
  ] = useState(-1);

  useEffect(() => {
    const compileSolver = async () => {
      try {
        await RubixService.compileSolver();
        console.log("Rubik's Cube solver compiled successfully");
      } catch (err) {
        console.error("Failed to compile Rubik's Cube solver:", err);
        setError("Failed to initialize solver. Please try again later.");
      }
    };
    compileSolver();
  }, []);

  const handleColorChange = (
    faceIndex,
    rowIndex,
    colIndex,
    value,
    isStartConfig
  ) => {
    const colorValue = parseInt(value);
    if (isNaN(colorValue) || colorValue < 0 || colorValue > 5) return;

    if (isStartConfig) {
      const newStartState = JSON.parse(JSON.stringify(startState));
      newStartState[faceIndex][rowIndex][colIndex] = colorValue;
      setStartState(newStartState);
    } else {
      const newFinalState = JSON.parse(JSON.stringify(finalState));
      newFinalState[faceIndex][rowIndex][colIndex] = colorValue;
      setFinalState(newFinalState);
    }
  };

  const validateCubeState = (state) => {
    const colorCounts = { 0: 0, 1: 0, 2: 0, 3: 0, 4: 0, 5: 0 };

    for (let face = 0; face < 6; face++) {
      for (let row = 0; row < 3; row++) {
        for (let col = 0; col < 3; col++) {
          const color = state[face][row][col];
          if (color >= 0 && color <= 5) {
            colorCounts[color]++;
          } else {
            return {
              valid: false,
              message: `Invalid color value found: ${color}`,
            };
          }
        }
      }
    }

    for (let color = 0; color <= 5; color++) {
      if (colorCounts[color] !== 9) {
        return {
          valid: false,
          message: `Invalid configuration: Color ${color} (${COLORS[color].name}) appears ${colorCounts[color]} times. Each color must appear exactly 9 times.`,
        };
      }
    }

    return { valid: true };
  };

  const solveCube = async () => {
    setLoading(true);
    setError(null);
    setSolutionStepsForDisplay([]);
    setCurrentDisplayedSolutionStepIndex(-1);
    setTrigger3DMoveCounter(0);

    const startStateValidation = validateCubeState(startState);
    if (!startStateValidation.valid) {
      setError(`Initial state error: ${startStateValidation.message}`);
      toast({
        variant: "destructive",
        title: "Invalid Initial State",
        description: startStateValidation.message,
      });
      setLoading(false);
      return;
    }

    const finalStateValidation = validateCubeState(finalState);
    if (!finalStateValidation.valid) {
      setError(`Target state error: ${finalStateValidation.message}`);
      toast({
        variant: "destructive",
        title: "Invalid Target State",
        description: finalStateValidation.message,
      });
      setLoading(false);
      return;
    }

    try {
      const result = await RubixService.solvePuzzle(startState, finalState);
      const moves = result.output
        .trim()
        .split(" ")
        .filter((move) => move !== "");
      const descriptiveMoves = moves.map((move) => ({
        notation: move,
        description: MOVE_DESCRIPTIONS[move] || `Unknown move: ${move}`,
      }));

      setSolutionStepsForDisplay(descriptiveMoves);

      if (descriptiveMoves.length > 0) {
        setSimulationInitialState(cloneCubeState(startState));
        setSimulationSolutionMoves(moves);
        setCurrentDisplayedSolutionStepIndex(-1);
        setSimulationViewActive(true);
        toast({
          title: "Cube Solution Found",
          description: "Press 'Next Move' to see the simulation in 3D.",
        });
      } else {
        toast({
          title: "Cube Solution",
          description: "The cube is already solved or no moves are required.",
        });
        setSimulationViewActive(false);
      }
    } catch (err) {
      setError(err.message);
      toast({
        variant: "destructive",
        title: "Error",
        description: err.message,
      });
    } finally {
      setLoading(false);
    }
  };

  const handleNext3DMove = () => {
    if (
      currentDisplayedSolutionStepIndex <
      simulationSolutionMoves.length - 1
    ) {
      setTrigger3DMoveCounter((prev) => prev + 1);
      setCurrentDisplayedSolutionStepIndex((prev) => prev + 1);
    }
  };

  const handlePrevious3DMove = () => {
    if (currentDisplayedSolutionStepIndex > -1) {
      setTrigger3DMoveCounter((prev) => prev - 1);
      setCurrentDisplayedSolutionStepIndex((prev) => prev - 1);
    }
  };

  const handleBackToConfiguration = () => {
    setSimulationViewActive(false);
    setSimulationInitialState(null);
    setSimulationSolutionMoves([]);
    setCurrentDisplayedSolutionStepIndex(-1);
    setTrigger3DMoveCounter(0);
    toast({
      title: "Exited Simulation",
      description: "Returned to cube configuration.",
    });
  };

  const renderFace = (faceIndex, faceName, isStartConfigDisplay = true) => {
    const stateToRender = isStartConfigDisplay ? startState : finalState;

    return (
      <div className="mb-4">
        <h3 className="text-sm font-medium mb-2">{faceName}</h3>
        <div className="grid grid-cols-3 gap-1 w-[120px]">
          {[0, 1, 2].map((rowIndex) =>
            [0, 1, 2].map((colIndex) => {
              const colorValue = stateToRender[faceIndex][rowIndex][colIndex];
              const color = COLORS[colorValue];

              return (
                <div
                  key={`${faceIndex}-${rowIndex}-${colIndex}`}
                  className={`w-9 h-9 flex items-center justify-center border ${color.border} ${color.bg} ${color.text} cursor-pointer`}
                  onClick={() => {
                    const newValue =
                      (stateToRender[faceIndex][rowIndex][colIndex] + 1) % 6;
                    handleColorChange(
                      faceIndex,
                      rowIndex,
                      colIndex,
                      newValue,
                      isStartConfigDisplay
                    );
                  }}
                >
                  {stateToRender[faceIndex][rowIndex][colIndex]}
                </div>
              );
            })
          )}
        </div>
      </div>
    );
  };

  return (
    <div className="space-y-6">
      <h2 className="text-xl font-bold">Rubik's Cube Solver</h2>
      {/* {!simulationViewActive ? ( */}
      <>
        <div className="p-4 bg-muted/20 rounded-lg">
          <h3 className="font-medium mb-2">Instructions:</h3>
          <ul className="list-disc pl-5 space-y-1 text-sm">
            <li>Click on any square to cycle through the colors (0-5)</li>
            <li>Set up your initial and target cube configurations</li>
            <li>
              Click "Solve Cube" to find the solution and start 3D simulation
            </li>
          </ul>
          <div className="mt-3 grid grid-cols-3 gap-2">
            {Object.entries(COLORS).map(([value, color]) => (
              <div key={value} className="flex items-center space-x-1">
                <div
                  className={`w-4 h-4 ${color.bg} ${color.border} border`}
                ></div>
                <span className="text-xs">
                  {value}: {color.name}
                </span>
              </div>
            ))}
          </div>
        </div>

        <div className="grid md:grid-cols-2 gap-8">
          <div>
            <h3 className="text-lg font-medium mb-3">Initial State</h3>
            <div className="grid grid-cols-3 gap-4">
              {renderFace(0, "Top Face (0)", true)}
              {renderFace(1, "Front Face (1)", true)}
              {renderFace(2, "Left Face (2)", true)}
              {renderFace(3, "Back Face (3)", true)}
              {renderFace(4, "Right Face (4)", true)}
              {renderFace(5, "Bottom Face (5)", true)}
            </div>
          </div>
          <div>
            <h3 className="text-lg font-medium mb-3">Target State</h3>
            <div className="grid grid-cols-3 gap-4">
              {renderFace(0, "Top Face (0)", false)}
              {renderFace(1, "Front Face (1)", false)}
              {renderFace(2, "Left Face (2)", false)}
              {renderFace(3, "Back Face (3)", false)}
              {renderFace(4, "Right Face (4)", false)}
              {renderFace(5, "Bottom Face (5)", false)}
            </div>
          </div>
        </div>

        <Button className="w-full" onClick={solveCube} disabled={loading}>
          {loading ? "Solving..." : "Solve Cube & Start 3D Simulation"}
        </Button>
      </>
      <>
        <div className="p-4 bg-muted/30 rounded-lg space-y-3">
          <div className="flex space-x-2 mt-4">
            <Button
              onClick={handlePrevious3DMove}
              disabled={loading || currentDisplayedSolutionStepIndex <= -1}
              variant="outline"
            >
              Previous Move
            </Button>
            <Button
              onClick={handleNext3DMove}
              disabled={
                loading ||
                currentDisplayedSolutionStepIndex >=
                  simulationSolutionMoves.length - 1
              }
              variant="outline"
            >
              Next Move
            </Button>
            <Button
              onClick={handleBackToConfiguration}
              disabled={loading}
              variant="secondary"
            >
              Back to Configuration
            </Button>
          </div>
          {currentDisplayedSolutionStepIndex > -1 &&
            solutionStepsForDisplay[currentDisplayedSolutionStepIndex] && (
              <p className="text-sm text-muted-foreground mt-2">
                Current Move ({currentDisplayedSolutionStepIndex + 1}/
                {simulationSolutionMoves.length}):{" "}
                <strong>
                  {
                    solutionStepsForDisplay[currentDisplayedSolutionStepIndex]
                      .notation
                  }
                </strong>{" "}
                -{" "}
                {
                  solutionStepsForDisplay[currentDisplayedSolutionStepIndex]
                    .description
                }
              </p>
            )}
          {currentDisplayedSolutionStepIndex === -1 &&
            simulationSolutionMoves.length > 0 && (
              <p className="text-sm text-muted-foreground mt-2">
                Ready to start. {simulationSolutionMoves.length} moves in
                solution.
              </p>
            )}
        </div>
      </>
      {/* )} */}
      {error && (
        <div className="p-4 bg-red-100 text-red-800 rounded-lg">
          <h3 className="font-medium mb-2">Error:</h3>
          <p>{error}</p>
        </div>
      )}
      {simulationViewActive && solutionStepsForDisplay.length > 0 && (
        <div className="p-4 bg-muted rounded-lg mt-4">
          <h3 className="font-medium mb-2">Full Solution Steps:</h3>
          <ul className="list-disc pl-5 space-y-1 text-sm">
            {solutionStepsForDisplay.map((move, index) => (
              <li
                key={index}
                className={
                  index === currentDisplayedSolutionStepIndex
                    ? "font-bold text-primary"
                    : ""
                }
              >
                {move.notation}: {move.description}
              </li>
            ))}
          </ul>
        </div>
      )}
    </div>
  );
}
