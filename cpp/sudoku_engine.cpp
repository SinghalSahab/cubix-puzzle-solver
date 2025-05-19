#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <random>
#include <chrono>
#include <iomanip>
using namespace std;

class SudokuEngine
{
private:
    bool isValid(const vector<vector<int> > &grid, int row, int col, int num)
    {
        for (int x = 0; x < 9; x++)
            if (grid[row][x] == num)
                return false;

        for (int x = 0; x < 9; x++)
            if (grid[x][col] == num)
                return false;

        int startRow = row - row % 3, startCol = col - col % 3;
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                if (grid[i + startRow][j + startCol] == num)
                    return false;

        return true;
    }

    bool solveSudoku(vector<vector<int> > &grid)
    {
        int row = -1, col = -1;
        bool isEmpty = false;

        for (int i = 0; i < 9; i++)
        {
            for (int j = 0; j < 9; j++)
            {
                if (grid[i][j] == 0)
                {
                    row = i;
                    col = j;
                    isEmpty = true;
                    break;
                }
            }
            if (isEmpty)
                break;
        }

        if (!isEmpty)
            return true;

        for (int num = 1; num <= 9; num++)
        {
            if (isValid(grid, row, col, num))
            {
                grid[row][col] = num;
                if (solveSudoku(grid))
                    return true;
                grid[row][col] = 0;
            }
        }
        return false;
    }

    vector<vector<int> > parseBoard(const vector<int> &values)
    {
        if (values.size() != 81)
        {
            throw runtime_error("Invalid board size");
        }

        vector<vector<int> > grid(9, vector<int>(9, 0));
        for (int i = 0; i < 9; i++)
        {
            for (int j = 0; j < 9; j++)
            {
                int val = values[i * 9 + j];
                if (val < 0 || val > 9)
                {
                    throw runtime_error("Invalid value in board");
                }
                grid[i][j] = val;
            }
        }
        return grid;
    }

    string gridToString(const vector<vector<int> > &grid)
    {
        stringstream result;
        for (int i = 0; i < 9; i++)
        {
            for (int j = 0; j < 9; j++)
            {
                result << grid[i][j];
                if (i != 8 || j != 8)
                    result << ",";
            }
        }
        return result.str();
    }

public:
    string solve(const vector<int>& vals) {
        try {
            vector<vector<int> > grid = parseBoard(vals);
            
            if(!solveSudoku(grid)) {
                return "Error: No solution exists";
            }
            return gridToString(grid);
        } catch (const exception& e) {
            return string("Error: ") + e.what();
        }
    }

    string getHint(const vector<int>& vals, int row, int col) {
        try {
            if(row < 0 || row >= 9 || col < 0 || col >= 9) {
                return "Error: Invalid position";
            }

            vector<vector<int> > grid = parseBoard(vals);
            if(grid[row][col] != 0) {
                return "Error: Cell already filled";
            }

            vector<vector<int> > sol = grid;
            if(!solveSudoku(sol)) {
                return "Error: Board has no solution";
            }

            return to_string(sol[row][col]);
        } catch (const exception& e) {
            return string("Error: ") + e.what();
        }
    }

    string checkValidity(const vector<int>& vals) {
        try {
            vector<vector<int> > grid = parseBoard(vals);
            
            for(int i = 0; i < 9; i++) {
                for(int j = 0; j < 9; j++) {
                    if(grid[i][j] != 0) {
                        int t = grid[i][j];
                        grid[i][j] = 0;
                        if(!isValid(grid, i, j, t)) {
                            return "Invalid";
                        }
                        grid[i][j] = t;
                    }
                }
            }
            return "Valid";
        } catch (const exception& e) {
            return string("Error: ") + e.what();
        }
    }
};

int main(int argc, char *argv[])
{
    if (argc < 83)
    {
        cerr << "Usage: " << argv[0] << " <command> <81 board values> [row col]" << endl;
        return 1;
    }

    try
    {
        vector<int> board;
        for (int i = 2; i < 83; i++)
        {
            board.push_back(stoi(argv[i]));
        }

        SudokuEngine engine;
        string command = argv[1];

        if (command == "solve")
        {
            cout << engine.solve(board) << endl;
        }
        else if (command == "check")
        {
            cout << engine.checkValidity(board) << endl;
        }
        else if (command == "hint")
        {
            if (argc < 85)
            {
                cerr << "Error: Not enough arguments for hint command" << endl;
                return 1;
            }
            int row = stoi(argv[83]);
            int col = stoi(argv[84]);
            cout << engine.getHint(board, row, col) << endl;
        }
        else
        {
            cerr << "Error: Unknown command: " << command << endl;
            return 1;
        }
    }
    catch (const exception &e)
    {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}
