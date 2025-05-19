"use client";
import React, { useMemo, useState, useEffect } from "react";
import { Canvas } from "@react-three/fiber";
import { OrbitControls, Box } from "@react-three/drei";
import * as THREE from "three";

const THREE_COLORS = {
  0: new THREE.Color(0xffffff), // White
  1: new THREE.Color(0xffa500), // Orange
  2: new THREE.Color(0x00dd00), // Brighter Green
  3: new THREE.Color(0xff0000), // Red
  4: new THREE.Color(0x0000ff), // Blue
  5: new THREE.Color(0xffff00), // Yellow
  default: new THREE.Color(0xff00ff), // Magenta for debugging default/missing faces
};

const CUBIE_SIZE = 1;
const CUBIE_SPACING = 0.05;
const CUBE_DIMENSION = 3; // Explicitly define CUBE_DIMENSION

const cloneCubeState = (state) => {
  if (!state) return null;
  return JSON.parse(JSON.stringify(state));
};

const rotateFaceClockwise = (face) => {
  const newFace = JSON.parse(JSON.stringify(face));
  newFace[0][0] = face[0][2];
  newFace[0][2] = face[2][2];
  newFace[2][2] = face[2][0];
  newFace[2][0] = face[0][0];
  newFace[0][1] = face[1][2];
  newFace[1][2] = face[2][1];
  newFace[2][1] = face[1][0];
  newFace[1][0] = face[0][1];
  return newFace;
};

const rotateFaceAntiClockwise = (face) => {
  const newFace = JSON.parse(JSON.stringify(face));
  newFace[0][0] = face[2][0];
  newFace[0][2] = face[0][0];
  newFace[2][2] = face[0][2];
  newFace[2][0] = face[2][2];
  newFace[0][1] = face[1][0];
  newFace[1][2] = face[0][1];
  newFace[2][1] = face[1][2];
  newFace[1][0] = face[2][1];
  return newFace;
};

const applyMoveInternal = (currentCubeState, move) => {
  if (!currentCubeState) return null;
  let newState = cloneCubeState(currentCubeState);
  let oldState = cloneCubeState(currentCubeState); // For reading values before they are changed in newState

  switch (move) {
    case "U":
      newState[0] = rotateFaceClockwise(oldState[0]);
      for (let c = 0; c < 3; c++) {
        newState[1][0][c] = oldState[2][0][c]; // Front top gets Left top
        newState[2][0][c] = oldState[3][0][c]; // Left top gets Back top
        newState[3][0][c] = oldState[4][0][c]; // Back top gets Right top
        newState[4][0][c] = oldState[1][0][c]; // Right top gets Front top
      }
      break;
    case "U'":
      newState[0] = rotateFaceAntiClockwise(oldState[0]);
      for (let c = 0; c < 3; c++) {
        newState[1][0][c] = oldState[4][0][c]; // Front top gets Right top
        newState[2][0][c] = oldState[1][0][c]; // Left top gets Front top
        newState[3][0][c] = oldState[2][0][c]; // Back top gets Left top
        newState[4][0][c] = oldState[3][0][c]; // Right top gets Back top
      }
      break;

    case "D":
      newState[5] = rotateFaceClockwise(oldState[5]);
      for (let c = 0; c < 3; c++) {
        newState[1][2][c] = oldState[4][2][c]; // Front bottom gets Right bottom
        newState[2][2][c] = oldState[1][2][c]; // Left bottom gets Front bottom
        newState[3][2][c] = oldState[2][2][c]; // Back bottom gets Left bottom
        newState[4][2][c] = oldState[3][2][c]; // Right bottom gets Back bottom
      }
      break;
    case "D'":
      newState[5] = rotateFaceAntiClockwise(oldState[5]);
      for (let c = 0; c < 3; c++) {
        newState[1][2][c] = oldState[2][2][c]; // Front bottom gets Left bottom
        newState[2][2][c] = oldState[3][2][c]; // Left bottom gets Back bottom
        newState[3][2][c] = oldState[4][2][c]; // Back bottom gets Right bottom
        newState[4][2][c] = oldState[1][2][c]; // Right bottom gets Front bottom
      }
      break;

    case "L":
      newState[2] = rotateFaceClockwise(oldState[2]);
      for (let r = 0; r < 3; r++) {
        newState[0][r][0] = oldState[3][2 - r][2]; // Up left gets Back right (inverted)
        newState[1][r][0] = oldState[0][r][0]; // Front left gets Up left
        newState[5][r][0] = oldState[1][r][0]; // Down left gets Front left
        newState[3][2 - r][2] = oldState[5][r][0]; // Back right (inverted) gets Down left
      }
      break;
    case "L'":
      newState[2] = rotateFaceAntiClockwise(oldState[2]);
      for (let r = 0; r < 3; r++) {
        newState[0][r][0] = oldState[1][r][0]; // Up left gets Front left
        newState[1][r][0] = oldState[5][r][0]; // Front left gets Down left
        newState[5][r][0] = oldState[3][2 - r][2]; // Down left gets Back right (inverted)
        newState[3][2 - r][2] = oldState[0][r][0]; // Back right (inverted) gets Up left
      }
      break;

    case "R":
      newState[4] = rotateFaceClockwise(oldState[4]);
      for (let r = 0; r < 3; r++) {
        newState[0][r][2] = oldState[1][r][2]; // Up right gets Front right
        newState[1][r][2] = oldState[5][r][2]; // Front right gets Down right
        newState[5][r][2] = oldState[3][2 - r][0]; // Down right gets Back left (inverted)
        newState[3][2 - r][0] = oldState[0][r][2]; // Back left (inverted) gets Up right
      }
      break;
    case "R'":
      newState[4] = rotateFaceAntiClockwise(oldState[4]);
      for (let r = 0; r < 3; r++) {
        newState[0][r][2] = oldState[3][2 - r][0]; // Up right gets Back left (inverted)
        newState[1][r][2] = oldState[0][r][2]; // Front right gets Up right
        newState[5][r][2] = oldState[1][r][2]; // Down right gets Front right
        newState[3][2 - r][0] = oldState[5][r][2]; // Back left (inverted) gets Down right
      }
      break;
    case "F":
      newState[1] = rotateFaceClockwise(oldState[1]);
      for (let i = 0; i < 3; i++) {
        newState[0][2][i] = oldState[2][2 - i][2]; // Up bottom row gets Left right col (inverted)
        newState[2][i][2] = oldState[5][0][i]; // Left right col gets Down top row
        newState[5][0][i] = oldState[4][2 - i][0]; // Down top row gets Right left col (inverted)
        newState[4][i][0] = oldState[0][2][i]; // Right left col gets Up bottom row
      }
      break;
    case "F'":
      newState[1] = rotateFaceAntiClockwise(oldState[1]);
      for (let i = 0; i < 3; i++) {
        newState[0][2][i] = oldState[4][i][0]; // Up bottom row gets Right left col
        newState[2][i][2] = oldState[0][2][2 - i]; // Left right col gets Up bottom row (inverted)
        newState[5][0][i] = oldState[2][i][2]; // Down top row gets Left right col
        newState[4][2 - i][0] = oldState[5][0][2 - i]; // Right left col (inverted) gets Down top row (inverted)
      }
      break;

    case "B":
      newState[3] = rotateFaceClockwise(oldState[3]);
      for (let i = 0; i < 3; i++) {
        newState[0][0][i] = oldState[4][i][2]; // Up top row gets Right right col
        newState[4][2 - i][2] = oldState[5][2][2 - i]; // Right right col (inverted) gets Down bottom row (inverted)
        newState[5][2][i] = oldState[2][i][0]; // Down bottom row gets Left left col
        newState[2][2 - i][0] = oldState[0][0][2 - i]; // Left left col (inverted) gets Up top row (inverted)
      }
      break;
    case "B'":
      newState[3] = rotateFaceAntiClockwise(oldState[3]);
      for (let i = 0; i < 3; i++) {
        newState[0][0][i] = oldState[2][2 - i][0]; // Up top gets Left left col (col inverted)
        newState[2][i][0] = oldState[5][2][i]; // Left left col gets Down bottom row
        newState[5][2][i] = oldState[4][2 - i][2]; // Down bottom gets Right right col (col inverted)
        newState[4][i][2] = oldState[0][0][i]; // Right right col gets Up top row
      }
      break;

    case "U2":
      newState = applyMoveInternal(applyMoveInternal(oldState, "U"), "U");
      break;
    case "D2":
      newState = applyMoveInternal(applyMoveInternal(oldState, "D"), "D");
      break;
    case "L2":
      newState = applyMoveInternal(applyMoveInternal(oldState, "L"), "L");
      break;
    case "R2":
      newState = applyMoveInternal(applyMoveInternal(oldState, "R"), "R");
      break;
    case "F2":
      newState = applyMoveInternal(applyMoveInternal(oldState, "F"), "F");
      break;
    case "B2":
      newState = applyMoveInternal(applyMoveInternal(oldState, "B"), "B");
      break;

    default:
      console.warn(
        `applyMoveInternal: Unknown or unhandled move ${move}. State will not change.`
      );
  }
  return newState;
};

const getCubieFaceColors = (x, y, z, cubeStateArray) => {
  const colors = Array(6).fill(THREE_COLORS.default); // Default all to magenta first

  if (
    !cubeStateArray ||
    !Array.isArray(cubeStateArray) ||
    cubeStateArray.length !== 6
  ) {
    return colors;
  }

  for (let i = 0; i < 6; i++) {
    if (
      !Array.isArray(cubeStateArray[i]) ||
      cubeStateArray[i].length !== CUBE_DIMENSION ||
      !cubeStateArray[i].every(
        (row) => Array.isArray(row) && row.length === CUBE_DIMENSION
      )
    ) {
      return colors; // Return all magenta if any face is malformed
    }
  }

  let colorValue, faceNameForLog;

  if (y === CUBE_DIMENSION - 1) {
    faceNameForLog = "Top";
    colorValue = cubeStateArray[0][CUBE_DIMENSION - 1 - z][x];
    if (THREE_COLORS[colorValue] !== undefined) {
      colors[0] = THREE_COLORS[colorValue];
    } else {
      console.log(
        `Cubie (${x},${y},${z}) - ${faceNameForLog} (0,${
          CUBE_DIMENSION - 1 - z
        },${x}): raw value '${colorValue}', using DEFAULT (Magenta)`
      );
    }
  }
  if (y === 0) {
    faceNameForLog = "Bottom";
    colorValue = cubeStateArray[5][z][x];
    if (THREE_COLORS[colorValue] !== undefined) {
      colors[1] = THREE_COLORS[colorValue];
    } else {
      console.log(
        `Cubie (${x},${y},${z}) - ${faceNameForLog} (5,${z},${x}): raw value '${colorValue}', using DEFAULT (Magenta)`
      );
    }
  }

  if (x === CUBE_DIMENSION - 1) {
    faceNameForLog = "Right";
    colorValue =
      cubeStateArray[4][CUBE_DIMENSION - 1 - y][CUBE_DIMENSION - 1 - z];
    if (THREE_COLORS[colorValue] !== undefined) {
      colors[2] = THREE_COLORS[colorValue];
    } else {
      console.log(
        `Cubie (${x},${y},${z}) - ${faceNameForLog} (4,${
          CUBE_DIMENSION - 1 - y
        },${
          CUBE_DIMENSION - 1 - z
        }): raw value '${colorValue}', using DEFAULT (Magenta)`
      );
    }
  }
  if (x === 0) {
    faceNameForLog = "Left";
    colorValue = cubeStateArray[2][CUBE_DIMENSION - 1 - y][z];
    if (THREE_COLORS[colorValue] !== undefined) {
      colors[3] = THREE_COLORS[colorValue];
    } else {
      console.log(
        `Cubie (${x},${y},${z}) - ${faceNameForLog} (2,${
          CUBE_DIMENSION - 1 - y
        },${z}): raw value '${colorValue}', using DEFAULT (Magenta)`
      );
    }
  }

  if (z === CUBE_DIMENSION - 1) {
    faceNameForLog = "Front";
    colorValue = cubeStateArray[1][CUBE_DIMENSION - 1 - y][x];
    if (THREE_COLORS[colorValue] !== undefined) {
      colors[4] = THREE_COLORS[colorValue];
    } else {
      console.log(
        `Cubie (${x},${y},${z}) - ${faceNameForLog} (1,${
          CUBE_DIMENSION - 1 - y
        },${x}): raw value '${colorValue}', using DEFAULT (Magenta)`
      );
    }
  }
  if (z === 0) {
    faceNameForLog = "Back";
    colorValue =
      cubeStateArray[3][CUBE_DIMENSION - 1 - y][CUBE_DIMENSION - 1 - x];
    if (THREE_COLORS[colorValue] !== undefined) {
      colors[5] = THREE_COLORS[colorValue];
    } else {
      console.log(
        `Cubie (${x},${y},${z}) - ${faceNameForLog} (3,${
          CUBE_DIMENSION - 1 - y
        },${
          CUBE_DIMENSION - 1 - x
        }): raw value '${colorValue}', using DEFAULT (Magenta)`
      );
    }
  }

  return colors;
};

const Cubie = ({ position, cubeStateArray, localCoords }) => {
  const faceColors = useMemo(
    () =>
      getCubieFaceColors(
        localCoords.x,
        localCoords.y,
        localCoords.z,
        cubeStateArray
      ),
    [localCoords, cubeStateArray] // Original order: Top, Bottom, Right, Left, Front, Back
  );

  const orderedMaterials = useMemo(() => {
    if (faceColors && faceColors.length === 6) {
      const [
        topColor,
        bottomColor,
        rightColor,
        leftColor,
        frontColor,
        backColor,
      ] = faceColors;
      // Box component material order: +X (Right), -X (Left), +Y (Top), -Y (Bottom), +Z (Front), -Z (Back)
      return [
        new THREE.MeshStandardMaterial({
          color: rightColor,
          side: THREE.DoubleSide,
        }), // Right (+X)
        new THREE.MeshStandardMaterial({
          color: leftColor,
          side: THREE.DoubleSide,
        }), // Left (-X)
        new THREE.MeshStandardMaterial({
          color: topColor,
          side: THREE.DoubleSide,
        }), // Top (+Y)
        new THREE.MeshStandardMaterial({
          color: bottomColor,
          side: THREE.DoubleSide,
        }), // Bottom (-Y)
        new THREE.MeshStandardMaterial({
          color: frontColor,
          side: THREE.DoubleSide,
        }), // Front (+Z)
        new THREE.MeshStandardMaterial({
          color: backColor,
          side: THREE.DoubleSide,
        }), // Back (-Z)
      ];
    }
    return Array(6).fill(
      new THREE.MeshStandardMaterial({
        color: THREE_COLORS.default,
        side: THREE.DoubleSide,
      })
    );
  }, [faceColors]);

  return (
    <Box
      args={[CUBIE_SIZE, CUBIE_SIZE, CUBIE_SIZE]}
      position={position}
      material={orderedMaterials} // Use the correctly ordered materials
    ></Box>
  );
};

const RubiksCube3D = ({
  initialStateArray,
  solutionMoves,
  currentMoveIndex,
}) => {
  const [displayableCubeState, setDisplayableCubeState] = useState(null);

  useEffect(() => {
    if (!initialStateArray) {
      setDisplayableCubeState(null);
      return;
    }

    let newDisplayState;
    if (currentMoveIndex === -1) {
      newDisplayState = cloneCubeState(initialStateArray);
    } else if (
      solutionMoves &&
      solutionMoves.length > 0 &&
      currentMoveIndex < solutionMoves.length
    ) {
      let stateAfterMoves = cloneCubeState(initialStateArray);
      for (let i = 0; i <= currentMoveIndex; i++) {
        if (solutionMoves[i]) {
          stateAfterMoves = applyMoveInternal(
            stateAfterMoves,
            solutionMoves[i]
          );
        }
      }
      newDisplayState = stateAfterMoves;
    } else {
      newDisplayState = cloneCubeState(initialStateArray);
    }

    setDisplayableCubeState(newDisplayState);
  }, [initialStateArray, solutionMoves, currentMoveIndex]);

  useEffect(() => {
    if (displayableCubeState) {
      let hasProblem = false;
      for (let faceIdx = 0; faceIdx < 6; faceIdx++) {
        if (!displayableCubeState[faceIdx]) {
          hasProblem = true;
          break;
        }
        for (let rowIdx = 0; rowIdx < 3; rowIdx++) {
          if (!displayableCubeState[faceIdx][rowIdx]) {
            hasProblem = true;
            break;
          }
          for (let colIdx = 0; colIdx < 3; colIdx++) {
            const val = displayableCubeState[faceIdx][rowIdx][colIdx];
            if (
              val === undefined ||
              val === null ||
              typeof val !== "number" ||
              val < 0 ||
              val > 5
            ) {
              hasProblem = true;
            }
          }
          if (hasProblem) break;
        }
        if (hasProblem) break;
      }
    }
  }, [displayableCubeState]);

  const cubies = useMemo(() => {
    const result = [];
    const offset = ((CUBE_DIMENSION - 1) / 2) * (CUBIE_SIZE + CUBIE_SPACING);

    for (let x = 0; x < CUBE_DIMENSION; x++) {
      for (let y = 0; y < CUBE_DIMENSION; y++) {
        for (let z = 0; z < CUBE_DIMENSION; z++) {
          if (CUBE_DIMENSION === 3 && x === 1 && y === 1 && z === 1) {
            continue; // Skip inner cubie
          }
          result.push({
            id: `${x}-${y}-${z}`,
            position: [
              x * (CUBIE_SIZE + CUBIE_SPACING) - offset,
              y * (CUBIE_SIZE + CUBIE_SPACING) - offset,
              z * (CUBIE_SIZE + CUBIE_SPACING) - offset,
            ],
            localCoords: { x, y, z },
          });
        }
      }
    }
    return result;
  }, []); // Cubie positions are static

  if (!displayableCubeState || displayableCubeState.length !== 6) {
    return (
      <div className="w-full h-64 flex items-center justify-center bg-gray-200 text-red-500">
        Error: Invalid cube state for 3D display or not initialized.
      </div>
    );
  }

  if (
    displayableCubeState.some(
      (face) =>
        !face ||
        face.length !== 3 ||
        face.some((row) => !row || row.length !== 3)
    )
  ) {
    return (
      <div className="w-full h-64 flex items-center justify-center bg-gray-200 text-red-500">
        Error: Invalid face/row structure for 3D display.
      </div>
    );
  }

  return (
    <Canvas
      style={{ width: "100%", height: "400px", background: "#e0e0e0" }}
      camera={{ position: [5, 5, 5], fov: 50 }}
    >
      <ambientLight intensity={0.9} />
      <directionalLight position={[10, 10, 10]} intensity={0.7} />
      <directionalLight position={[-10, -10, -5]} intensity={0.4} />
      <directionalLight position={[0, 10, 0]} intensity={0.6} />
      <directionalLight position={[0, -10, 0]} intensity={0.3} />
      <group>
        {cubies.map((cubie) => (
          <Cubie
            key={cubie.id}
            position={cubie.position}
            cubeStateArray={displayableCubeState}
            localCoords={cubie.localCoords}
          />
        ))}
      </group>
      <OrbitControls />
    </Canvas>
  );
};

export default RubiksCube3D;
