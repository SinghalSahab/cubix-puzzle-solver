# Cubix - A Puzzle Solving Tool

## Project Synopsis
Cubix is a web-based puzzle solver designed to solve multiple types of puzzles, including:

### Rubik's Cube  
![Rubik's Cube](https://github.com/user-attachments/assets/6b4dc7f8-e67e-436e-95f9-209884150401)

### Number Slider Puzzle (15-puzzle)  
![Number Slider Puzzle](https://github.com/user-attachments/assets/401cb300-37a9-44b8-9d69-c01c75993ac0)

### Sudoku  
![Sudoku](https://github.com/user-attachments/assets/5f188c5d-d713-4a61-8473-c0bbf58d9973)

## Project Objective
The objective of this project is to provide an intuitive and efficient puzzle-solving tool. The frontend is developed using React to offer a user-friendly interface for puzzle input and solution visualization. All puzzle solvers are compiled to WebAssembly, allowing the C++ solvers to run directly in the browser without requiring API calls.

## Project Architecture

### Frontend (React)
**User Interface:**
- Input fields for Sudoku, Number Slider, and Rubik's Cube.
- Toggle between Play Mode (manual play) and Solver Mode (automated solving).
- Step-by-step solution visualization.

**State Management:**
- Uses React hooks to handle puzzle states.

### WebAssembly Integration
- C++ solvers are compiled to WebAssembly using Emscripten.
- Runs directly in the browser without requiring server API calls.
- Provides optimal performance for complex algorithmic operations.
- Enables offline solving capabilities.

### Solving Algorithms

#### Rubik's Cube Solver
- Utilizes an IDA* algorithm for optimal solutions.
- Represents the cube using a 3D array data structure.
- Compiled to WebAssembly for client-side execution.

#### Number Slider Puzzle (15-puzzle)
- Uses A* Search Algorithm with Manhattan Distance heuristic for optimal pathfinding.
- Integrated solvability check using inversion count.
- Compiled to WebAssembly for fast performance in browser.

#### Sudoku Solver
- Implements the Backtracking Algorithm with constraint propagation.
- Features validity checking and hint generation.
- Compiled to WebAssembly for seamless browser execution.

## Setup Instructions

### Prerequisites

#### WebAssembly Requirements
1. Install Emscripten SDK:
   ```bash
   git clone https://github.com/emscripten-core/emsdk.git
   cd emsdk
   ./emsdk install latest
   ./emsdk activate latest
   source ./emsdk_env.sh  # On Windows use: emsdk_env.bat
   ```

#### Windows
1. Install MinGW-w64 (includes g++):
   - Download from: https://www.mingw-w64.org/
   - Add MinGW-w64 bin directory to system PATH

#### macOS
1. Install Xcode Command Line Tools (includes g++):
   ```bash
   xcode-select --install
   ```

#### Linux
1. Install g++:
   ```bash
   sudo apt update
   sudo apt install g++  # For Ubuntu/Debian
   # OR
   sudo dnf install gcc-c++  # For Fedora
   ```

### Installation
1. Clone the repository:
   ```bash
   git clone https://github.com/KshKnsl/cubix.git
   cd cubix
   ```

2. Install dependencies:
   ```bash
   npm install
   ```
   This will automatically compile the C++ engine during installation.

3. Start the development server:
   ```bash
   npm run dev
   ```

4. Open http://localhost:3000 in your browser

### Troubleshooting
- If you encounter compilation errors:
  1. Ensure g++ is properly installed: `g++ --version`
  2. Manually compile the engine: `npm run compile-engine`
  3. Check the logs in `%TEMP%/cubix-sudoku` (Windows) or `/tmp/cubix-sudoku` (Mac/Linux)

## Getting Started

### Prerequisites
- Node.js
- npm or yarn
- C++ compiler

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
