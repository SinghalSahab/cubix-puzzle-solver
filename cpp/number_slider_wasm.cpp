#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <string>
#include <vector>
#include <queue>
#include <unordered_set>
#include <sstream>
#include <algorithm>

using namespace emscripten;

class NumberSliderEngine {
private:
    struct State {
        std::vector<int> board;
        int emptyPos;
        int size;
        int moves;
        std::string path;

        State(const std::vector<int>& b, int s, int e, int m, std::string p) 
            : board(b), size(s), emptyPos(e), moves(m), path(p) {}

        std::string toString() const {
            std::stringstream ss;
            for (int val : board) {
                ss << val << ",";
            }
            return ss.str();
        }
    };

    int calculateManhattan(const std::vector<int>& board, int size) {
        int distance = 0;
        for (int i = 0; i < board.size(); i++) {
            if (board[i] != 0) {  
                int goalRow = (board[i] - 1) / size;
                int goalCol = (board[i] - 1) % size;
                int currentRow = i / size;
                int currentCol = i % size;
                distance += abs(currentRow - goalRow) + abs(currentCol - goalCol);
            }
        }
        return distance;
    }

    bool isSolvable(const std::vector<int>& board, int size) {
        int inversions = 0;
        for (int i = 0; i < board.size(); i++) {
            if (board[i] == 0) continue;  
            
            for (int j = i + 1; j < board.size(); j++) {
                if (board[j] == 0) continue;  
                
                if (board[i] > board[j]) {
                    inversions++;
                }
            }
        }

        if (size % 2 == 1) {
            return inversions % 2 == 0;
        } else {
            int emptyPos = std::find(board.begin(), board.end(), 0) - board.begin();
            int emptyRow = emptyPos / size;
            return (inversions + emptyRow) % 2 == 1;
        }
    }

    std::vector<int> getPossibleMoves(int emptyPos, int size) {
        std::vector<int> moves;
        int row = emptyPos / size;
        int col = emptyPos % size;
        
        if (row > 0) {
            moves.push_back(emptyPos - size);
        }
        
        if (row < size - 1) {
            moves.push_back(emptyPos + size);
        }
        
        if (col > 0) {
            moves.push_back(emptyPos - 1);
        }
        
        if (col < size - 1) {
            moves.push_back(emptyPos + 1);
        }
        
        return moves;
    }

    std::string getMoveDirection(int from, int to, int size) {
        int fromRow = from / size;
        int fromCol = from % size;
        int toRow = to / size;
        int toCol = to % size;
        
        if (fromRow > toRow) return "U";  
        if (fromRow < toRow) return "D";  
        if (fromCol > toCol) return "L";  
        if (fromCol < toCol) return "R";  
        return "";
    }

    bool isGoalState(const std::vector<int>& board) {
        for (int i = 0; i < board.size() - 1; i++) {
            if (board[i] != i + 1) {
                return false;
            }
        }
        return board.back() == 0;
    }

public:
    NumberSliderEngine() {}

    std::string solve(const std::vector<int>& inputBoard) {
        if (inputBoard.empty()) {
            return "Error: Empty board";
        }
        
        int size = static_cast<int>(sqrt(inputBoard.size()));
        if (size * size != inputBoard.size()) {
            return "Error: Board size must be a perfect square";
        }
        
        if (!isSolvable(inputBoard, size)) {
            return "Error: Puzzle is not solvable";
        }
        
        auto it = std::find(inputBoard.begin(), inputBoard.end(), 0);
        if (it == inputBoard.end()) {
            return "Error: No empty space found";
        }
        int emptyPos = std::distance(inputBoard.begin(), it);
        
        if (isGoalState(inputBoard)) {
            return "";  
        }
        
        auto compare = [this](const State& a, const State& b) {
            return a.moves + calculateManhattan(a.board, a.size) > 
                   b.moves + calculateManhattan(b.board, b.size);
        };
        
        std::priority_queue<State, std::vector<State>, decltype(compare)> queue(compare);
        std::unordered_set<std::string> visited;
        
        State initialState(inputBoard, size, emptyPos, 0, "");
        queue.push(initialState);
        visited.insert(initialState.toString());
        
        while (!queue.empty()) {
            State current = queue.top();
            queue.pop();
            
            if (isGoalState(current.board)) {
                return current.path;
            }
            
            std::vector<int> possibleMoves = getPossibleMoves(current.emptyPos, size);
            
            for (int newPos : possibleMoves) {
                std::vector<int> newBoard = current.board;
                std::swap(newBoard[current.emptyPos], newBoard[newPos]);
                
                std::string key = [&newBoard]() {
                    std::stringstream ss;
                    for (int val : newBoard) {
                        ss << val << ",";
                    }
                    return ss.str();
                }();
                
                if (visited.find(key) == visited.end()) {
                    std::string direction = getMoveDirection(current.emptyPos, newPos, size);
                    State nextState(newBoard, size, newPos, current.moves + 1, current.path + direction);
                    queue.push(nextState);
                    visited.insert(key);
                }
            }
        }
        
        return "Error: No solution found";
    }

    bool isValidMove(const std::vector<int>& board, const std::string& move) {
        if (board.empty() || move.empty()) {
            return false;
        }
        
        int size = static_cast<int>(sqrt(board.size()));
        if (size * size != board.size()) {
            return false;
        }
        
        auto it = std::find(board.begin(), board.end(), 0);
        if (it == board.end()) {
            return false;
        }
        int emptyPos = std::distance(board.begin(), it);
        
        int row = emptyPos / size;
        int col = emptyPos % size;
        
        if (move == "U" && row > 0) return true;
        if (move == "D" && row < size - 1) return true;
        if (move == "L" && col > 0) return true;
        if (move == "R" && col < size - 1) return true;
        
        return false;
    }

    std::string applyMove(const std::vector<int>& board, const std::string& move) {
        if (!isValidMove(board, move)) {
            return "Error: Invalid move";
        }
        
        std::vector<int> newBoard = board;
        
        int size = static_cast<int>(sqrt(newBoard.size()));
        
        int emptyPos = std::distance(newBoard.begin(), std::find(newBoard.begin(), newBoard.end(), 0));
        
        int newPos = emptyPos;
        if (move == "U") newPos = emptyPos - size;
        else if (move == "D") newPos = emptyPos + size;
        else if (move == "L") newPos = emptyPos - 1;
        else if (move == "R") newPos = emptyPos + 1;
        
        std::swap(newBoard[emptyPos], newBoard[newPos]);
        
        std::stringstream ss;
        for (size_t i = 0; i < newBoard.size(); ++i) {
            ss << newBoard[i];
            if (i < newBoard.size() - 1) {
                ss << ",";
            }
        }
        
        return ss.str();
    }

    std::string getHint(const std::vector<int>& board) {
        std::string solution = this->solve(board); // Corrected: Added this->
        if (solution.empty() || solution.substr(0, 5) == "Error") {
            return solution;
        }
        
        return solution.substr(0, 1);
    }

    bool isComplete(const std::vector<int>& board) {
        return this->isGoalState(board); // Corrected: Added this->
    }
};

EMSCRIPTEN_BINDINGS(number_slider_module) {
    class_<NumberSliderEngine>("NumberSliderEngine")
        .constructor<>()
        .function("solve", &NumberSliderEngine::solve)
        .function("isValidMove", &NumberSliderEngine::isValidMove)
        .function("applyMove", &NumberSliderEngine::applyMove)
        .function("getHint", &NumberSliderEngine::getHint)
        .function("isComplete", &NumberSliderEngine::isComplete);
    
    register_vector<int>("IntVector");
}