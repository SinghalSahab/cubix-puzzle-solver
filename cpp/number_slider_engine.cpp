#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>
#include <climits>
#include <algorithm>
using namespace std;

class Node
{
private:
    static const int N = 4;
    vector<vector<int> > board;
    int g;
    int h;
    int zr, zc;
    vector<pair<int, int> > path;
    int last_move;

    void locateZero()
    {
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; j++)
                if (board[i][j] == 0)
                {
                    zr = i;
                    zc = j;
                    return;
                }
    }

    int computeHeuristic() const
    {
        int d = 0;
        int c = 0;

        for (int i = 0; i < N; ++i)
        {
            for (int j = 0; j < N; ++j)
            {
                if (board[i][j] != 0)
                {
                    int tr = (board[i][j] - 1) / N;
                    int tc = (board[i][j] - 1) % N;
                    d += abs(i - tr) + abs(j - tc);
                }
            }
        }

        return d + c;
    }

public:
    Node(const vector<vector<int> > &b, int g_, const vector<pair<int, int> > &p, int last_move_ = -1)
        : board(b), g(g_), path(p), last_move(last_move_)
    {
        h = computeHeuristic();
        locateZero();
    }

    bool isGoal() const
    {
        int count = 1;
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; ++j)
                if (i == N - 1 && j == N - 1)
                {
                    if (board[i][j] != 0)
                        return false;
                }
                else
                {
                    if (board[i][j] != count++)
                        return false;
                }
        return true;
    }

    int f() const
    {
        return g + h;
    }

    string serialize() const
    {
        string key;
        for (size_t i = 0; i < board.size(); ++i)
        {
            for (size_t j = 0; j < board[i].size(); ++j)
            {
                key += to_string(board[i][j]) + ",";
            }
        }
        return key;
    }

    const vector<vector<int> > &getBoard() const { return board; }
    int getZeroRow() const { return zr; }
    int getZeroCol() const { return zc; }
    const vector<pair<int, int> > &getPath() const { return path; }
    int getLastMove() const { return last_move; }
    int getG() const { return g; }
    friend class FifteenPuzzleSolver;
};

class FifteenPuzzleSolver
{
private:
    static const int N = 4;
    vector<pair<int, int> > directions;

    bool isSolvable(const vector<vector<int> > &board) const
    {
        vector<int> flat;
        int zero_row = 0;
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; ++j)
            {
                if (board[i][j] == 0)
                    zero_row = i;
                else
                    flat.push_back(board[i][j]);
            }

        int inversions = 0;
        for (int i = 0; i < flat.size(); ++i)
            for (int j = i + 1; j < flat.size(); ++j)
                if (flat[i] > flat[j])
                    ++inversions;

        int row_from_bottom = N - zero_row;

        return (row_from_bottom % 2 == 1) ? (inversions % 2 == 0) : (inversions % 2 == 1);
    }

    int ida(Node &n, int th, int &nTh, int lm, unordered_set<string> &vis) const
    {
        int f = n.f();
        if (f > th)
        {
            nTh = min(nTh, f);
            return -1;
        }

        if (n.isGoal())
        {
            cout << "Solved in " << n.getG() << " moves.\n";
            for (size_t i = 0; i < n.getPath().size(); ++i)
                cout << "(" << n.getPath()[i].first << "," << n.getPath()[i].second << ") ";
            cout << "\n";
            return n.getG();
        }

        string sb = n.serialize();
        if (vis.count(sb))
        {
            return -1;
        }
        vis.insert(sb);

        for (int i = 0; i < directions.size(); ++i)
        {
            if (lm != -1)
            {
                if ((lm == 0 && i == 1) ||
                    (lm == 1 && i == 0) ||
                    (lm == 2 && i == 3) ||
                    (lm == 3 && i == 2))
                {
                    continue;
                }
            }

            int ni = n.getZeroRow() + directions[i].first;
            int nj = n.getZeroCol() + directions[i].second;

            if (ni >= 0 && ni < N && nj >= 0 && nj < N)
            {
                vector<vector<int> > newBoard = n.getBoard();
                swap(newBoard[n.getZeroRow()][n.getZeroCol()], newBoard[ni][nj]);
                vector<pair<int, int> > newPath = n.getPath();
                newPath.emplace_back(ni, nj);
                Node next(newBoard, n.getG() + 1, newPath, i);
                int result = ida(next, th, nTh, i, vis);
                if (result != -1)
                    return result;
            }
        }
        vis.erase(sb);
        return -1;
    }

public:
    FifteenPuzzleSolver()
    {
        directions.push_back(make_pair(-1, 0));
        directions.push_back(make_pair(1, 0));
        directions.push_back(make_pair(0, -1));
        directions.push_back(make_pair(0, 1));
    }

    void solve(const vector<vector<int> > &board)
    {
        if (!isSolvable(board))
        {
            return;
        }

        Node start(board, 0, vector<pair<int, int> >(), -1);
        int threshold = start.f();
        unordered_set<string> visited;

        while (true)
        {
            int next_threshold = INT_MAX;
            int result = ida(start, threshold, next_threshold, -1, visited);
            if (result != -1)
                return;
            if (next_threshold == INT_MAX)
            {
                cout << "No solution found.\n";
                return;
            }
            threshold = next_threshold;
        }
    }
};
int main(int argc, char *argv[])
{
    vector<vector<int> > board;

    if (argc >= 17)
    {
        board.resize(4, vector<int>(4));
        for (int i = 0; i < 16; ++i)
        {
            board[i / 4][i % 4] = stoi(argv[i + 1]);
        }
    }
    else
    {
        int default_board_data[4][4] = {{7, 13, 9, 12}, {8, 14, 5, 11}, {3, 2, 1, 15}, {0, 10, 6, 4}};
        board.resize(4, vector<int>(4));
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                board[i][j] = default_board_data[i][j];
            }
        }
        cerr << "Using default board. For custom board: " << argv[0] << " <16 board values>" << endl;
    }

    FifteenPuzzleSolver solver;
    solver.solve(board);
    return 0;
}