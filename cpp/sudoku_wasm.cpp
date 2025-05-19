#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <string>
#include <vector>
#include <sstream>

using namespace emscripten;

class SudokuEngine {
private:
    static const int SIZE = 9;
    static const int EMPTY = 0;
    static const int BOX_SIZE = 3;

    // Check if the number is valid in the given row
    bool isValidInRow(const std::vector<int>& board, int row, int num) {
        for (int col = 0; col < SIZE; col++) {
            if (board[row * SIZE + col] == num) {
                return false;
            }
        }
        return true;
    }

    // Check if the number is valid in the given column
    bool isValidInCol(const std::vector<int>& board, int col, int num) {
        for (int row = 0; row < SIZE; row++) {
            if (board[row * SIZE + col] == num) {
                return false;
            }
        }
        return true;
    }

    // Check if the number is valid in the 3x3 box
    bool isValidInBox(const std::vector<int>& board, int boxStartRow, int boxStartCol, int num) {
        for (int row = 0; row < BOX_SIZE; row++) {
            for (int col = 0; col < BOX_SIZE; col++) {
                if (board[(boxStartRow + row) * SIZE + (boxStartCol + col)] == num) {
                    return false;
                }
            }
        }
        return true;
    }

    // Check if the number is valid at the given position
    bool isValidPlacement(const std::vector<int>& board, int row, int col, int num) {
        return isValidInRow(board, row, num) &&
               isValidInCol(board, col, num) &&
               isValidInBox(board, row - row % BOX_SIZE, col - col % BOX_SIZE, num);
    }

    // Check if the board is valid
    bool isBoardValid(const std::vector<int>& board) {
        // Check rows
        for (int row = 0; row < SIZE; row++) {
            std::vector<bool> seen(SIZE + 1, false);
            for (int col = 0; col < SIZE; col++) {
                int val = board[row * SIZE + col];
                if (val != EMPTY) {
                    if (val < 1 || val > SIZE || seen[val]) {
                        return false;
                    }
                    seen[val] = true;
                }
            }
        }

        // Check columns
        for (int col = 0; col < SIZE; col++) {
            std::vector<bool> seen(SIZE + 1, false);
            for (int row = 0; row < SIZE; row++) {
                int val = board[row * SIZE + col];
                if (val != EMPTY) {
                    if (seen[val]) {
                        return false;
                    }
                    seen[val] = true;
                }
            }
        }

        // Check 3x3 boxes
        for (int boxStartRow = 0; boxStartRow < SIZE; boxStartRow += BOX_SIZE) {
            for (int boxStartCol = 0; boxStartCol < SIZE; boxStartCol += BOX_SIZE) {
                std::vector<bool> seen(SIZE + 1, false);
                for (int row = 0; row < BOX_SIZE; row++) {
                    for (int col = 0; col < BOX_SIZE; col++) {
                        int val = board[(boxStartRow + row) * SIZE + (boxStartCol + col)];
                        if (val != EMPTY) {
                            if (seen[val]) {
                                return false;
                            }
                            seen[val] = true;
                        }
                    }
                }
            }
        }

        return true;
    }

    // Solve the Sudoku board using backtracking
    bool solveBoard(std::vector<int>& board) {
        for (int row = 0; row < SIZE; row++) {
            for (int col = 0; col < SIZE; col++) {
                if (board[row * SIZE + col] == EMPTY) {
                    for (int num = 1; num <= SIZE; num++) {
                        if (isValidPlacement(board, row, col, num)) {
                            board[row * SIZE + col] = num;
                            
                            if (solveBoard(board)) {
                                return true;
                            }
                            
                            board[row * SIZE + col] = EMPTY; // Undo if it leads to no solution
                        }
                    }
                    return false; // Trigger backtracking if no number can be placed
                }
            }
        }
        return true; // Board is full and valid
    }

public:
    SudokuEngine() {}

    // Solve the Sudoku puzzle
    std::string solve(const std::vector<int>& inputBoard) {
        if (inputBoard.size() != SIZE * SIZE) {
            return "Error: Input board must have 81 cells";
        }
        
        // Check if the board is valid before solving
        if (!isBoardValid(inputBoard)) {
            return "Error: Input board is not valid";
        }
        
        // Make a copy to solve
        std::vector<int> board = inputBoard;
        
        if (solveBoard(board)) {
            // Convert the solved board to a comma-separated string
            std::stringstream ss;
            for (size_t i = 0; i < board.size(); ++i) {
                ss << board[i];
                if (i < board.size() - 1) {
                    ss << ",";
                }
            }
            return ss.str();
        } else {
            return "Error: No solution exists";
        }
    }

    // Check if the current board configuration is valid
    std::string checkValidity(const std::vector<int>& board) {
        if (board.size() != SIZE * SIZE) {
            return "Invalid: Board must have 81 cells";
        }
        
        if (isBoardValid(board)) {
            return "Valid: Board state is correct";
        } else {
            return "Invalid: Board has conflicts";
        }
    }

    // Get a hint for a specific cell
    std::string getHint(const std::vector<int>& inputBoard, int row, int col) {
        if (inputBoard.size() != SIZE * SIZE) {
            return "Error: Input board must have 81 cells";
        }
        
        if (row < 0 || row >= SIZE || col < 0 || col >= SIZE) {
            return "Error: Invalid row or column";
        }
        
        if (inputBoard[row * SIZE + col] != EMPTY) {
            return "Error: Cell is already filled";
        }
        
        // Check if the board is valid
        if (!isBoardValid(inputBoard)) {
            return "Error: Board is not valid";
        }
        
        // Make a copy to solve
        std::vector<int> board = inputBoard;
        
        if (solveBoard(board)) {
            // Return the value at the specified position
            return std::to_string(board[row * SIZE + col]);
        } else {
            return "Error: No solution exists";
        }
    }
};

// Binding code
EMSCRIPTEN_BINDINGS(sudoku_module) {
    class_<SudokuEngine>("SudokuEngine")
        .constructor<>()
        .function("solve", &SudokuEngine::solve)
        .function("checkValidity", &SudokuEngine::checkValidity)
        .function("getHint", &SudokuEngine::getHint);
    
    register_vector<int>("IntVector");
}