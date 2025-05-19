#include <iostream>
#include <fstream>
#include <stack>
#include <vector>
#include <limits>
#include <algorithm>
#include <map>
#include <string>
#include <numeric>
#include <set>

using namespace std;

void takeInput(int ***start, int ***goal, int argc, char *argv[])
{
    if (argc > 1 && (argc - 1) >= 108)
    {
        int argIndex = 1;
        for (int i = 0; i < 6; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                for (int k = 0; k < 3; k++)
                {
                    start[i][j][k] = atoi(argv[argIndex++]);
                }
            }
        }
        for (int i = 0; i < 6; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                for (int k = 0; k < 3; k++)
                {
                    goal[i][j][k] = atoi(argv[argIndex++]);
                }
            }
        }
    }
    else
    {
        int defaultStart[54] = {
            0, 0, 0, 0, 0, 1, 0, 2, 0,
            1, 1, 1, 1, 1, 2, 1, 0, 1,
            2, 2, 2, 2, 2, 2, 2, 3, 2,
            3, 3, 3, 3, 3, 3, 0, 3, 3,
            4, 4, 4, 4, 4, 4, 4, 4, 1,
            5, 5, 5, 5, 5, 5, 5, 5, 5};
        int defaultGoal[54] = {
            0, 0, 0, 0, 0, 0, 0, 0, 0,
            1, 1, 1, 1, 1, 1, 1, 1, 1,
            2, 2, 2, 2, 2, 2, 2, 2, 2,
            3, 3, 3, 3, 3, 3, 3, 3, 3,
            4, 4, 4, 4, 4, 4, 4, 4, 4,
            5, 5, 5, 5, 5, 5, 5, 5, 5};
        int startIdx = 0;
        int goalIdx = 0;
        for (int i = 0; i < 6; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                for (int k = 0; k < 3; k++)
                {
                    start[i][j][k] = defaultStart[startIdx++];
                    goal[i][j][k] = defaultGoal[goalIdx++];
                }
            }
        }
        cout << "Using default input values." << endl;
    }
}

void print(int ***arr)
{
    cout << "Printing " << endl;
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                cout << arr[i][j][k];
            }
            cout << endl;
        }
        cout << endl;
    }
    cout << endl;
}

int ***rA(int ***cube)
{
    int ***temp;
    temp = new int **[6];
    for (int i = 0; i < 6; i++)
    {
        temp[i] = new int *[3];
        for (int j = 0; j < 3; j++)
        {
            temp[i][j] = new int[3];
        }
    }
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                temp[i][j][k] = cube[i][j][k];
            }
        }
    }
    temp[4][0][1] = cube[4][1][0];
    temp[4][1][0] = cube[4][2][1];
    temp[4][2][1] = cube[4][1][2];
    temp[4][1][2] = cube[4][0][1];
    temp[4][0][0] = cube[4][2][0];
    temp[4][2][0] = cube[4][2][2];
    temp[4][2][2] = cube[4][0][2];
    temp[4][0][2] = cube[4][0][0];
    temp[0][0][2] = cube[3][0][2];
    temp[0][1][2] = cube[3][1][2];
    temp[0][2][2] = cube[3][2][2];
    temp[3][0][2] = cube[5][0][2];
    temp[3][1][2] = cube[5][1][2];
    temp[3][2][2] = cube[5][2][2];
    temp[5][0][2] = cube[1][0][2];
    temp[5][1][2] = cube[1][1][2];
    temp[5][2][2] = cube[1][2][2];
    temp[1][0][2] = cube[0][0][2];
    temp[1][1][2] = cube[0][1][2];
    temp[1][2][2] = cube[0][2][2];
    return temp;
}

int ***rC(int ***cube)
{
    int ***temp;
    temp = new int **[6];
    for (int i = 0; i < 6; i++)
    {
        temp[i] = new int *[3];
        for (int j = 0; j < 3; j++)
        {
            temp[i][j] = new int[3];
        }
    }
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                temp[i][j][k] = cube[i][j][k];
            }
        }
    }
    temp[4][0][1] = cube[4][1][2];
    temp[4][1][0] = cube[4][0][1];
    temp[4][2][1] = cube[4][1][0];
    temp[4][1][2] = cube[4][2][1];
    temp[4][0][0] = cube[4][0][2];
    temp[4][2][0] = cube[4][0][0];
    temp[4][2][2] = cube[4][2][0];
    temp[4][0][2] = cube[4][2][2];
    temp[0][0][2] = cube[1][0][2];
    temp[0][1][2] = cube[1][1][2];
    temp[0][2][2] = cube[1][2][2];
    temp[3][0][2] = cube[0][0][2];
    temp[3][1][2] = cube[0][1][2];
    temp[3][2][2] = cube[0][2][2];
    temp[5][0][2] = cube[3][0][2];
    temp[5][1][2] = cube[3][1][2];
    temp[5][2][2] = cube[3][2][2];
    temp[1][0][2] = cube[5][0][2];
    temp[1][1][2] = cube[5][1][2];
    temp[1][2][2] = cube[5][2][2];
    return temp;
}

int ***uC(int ***cube)
{
    int ***temp;
    temp = new int **[6];
    for (int i = 0; i < 6; i++)
    {
        temp[i] = new int *[3];
        for (int j = 0; j < 3; j++)
        {
            temp[i][j] = new int[3];
        }
    }
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                temp[i][j][k] = cube[i][j][k];
            }
        }
    }
    temp[0][0][1] = cube[0][1][2];
    temp[0][1][0] = cube[0][0][1];
    temp[0][2][1] = cube[0][1][0];
    temp[0][1][2] = cube[0][2][1];
    temp[0][0][0] = cube[0][0][2];
    temp[0][2][0] = cube[0][0][0];
    temp[0][2][2] = cube[0][2][0];
    temp[0][0][2] = cube[0][2][2];
    temp[2][0][0] = cube[1][0][0];
    temp[2][1][0] = cube[1][1][0];
    temp[2][2][0] = cube[1][2][0];
    temp[3][0][0] = cube[2][0][0];
    temp[3][1][0] = cube[2][1][0];
    temp[3][2][0] = cube[2][2][0];
    temp[4][0][0] = cube[3][0][0];
    temp[4][1][0] = cube[3][1][0];
    temp[4][2][0] = cube[3][2][0];
    temp[1][0][0] = cube[4][0][0];
    temp[1][1][0] = cube[4][1][0];
    temp[1][2][0] = cube[4][2][0];
    return temp;
}

int ***uA(int ***cube)
{
    int ***temp;
    temp = new int **[6];
    for (int i = 0; i < 6; i++)
    {
        temp[i] = new int *[3];
        for (int j = 0; j < 3; j++)
        {
            temp[i][j] = new int[3];
        }
    }
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                temp[i][j][k] = cube[i][j][k];
            }
        }
    }
    temp[0][0][1] = cube[0][1][0];
    temp[0][1][0] = cube[0][2][1];
    temp[0][2][1] = cube[0][1][2];
    temp[0][1][2] = cube[0][0][1];
    temp[0][0][0] = cube[0][2][0];
    temp[0][2][0] = cube[0][2][2];
    temp[0][2][2] = cube[0][0][2];
    temp[0][0][2] = cube[0][0][0];
    temp[2][0][0] = cube[3][0][0];
    temp[2][1][0] = cube[3][1][0];
    temp[2][2][0] = cube[3][2][0];
    temp[3][0][0] = cube[4][0][0];
    temp[3][1][0] = cube[4][1][0];
    temp[3][2][0] = cube[4][2][0];
    temp[4][0][0] = cube[1][0][0];
    temp[4][1][0] = cube[1][1][0];
    temp[4][2][0] = cube[1][2][0];
    temp[1][0][0] = cube[2][0][0];
    temp[1][1][0] = cube[2][1][0];
    temp[1][2][0] = cube[2][2][0];
    return temp;
}

int ***lC(int ***cube)
{
    int ***temp;
    temp = new int **[6];
    for (int i = 0; i < 6; i++)
    {
        temp[i] = new int *[3];
        for (int j = 0; j < 3; j++)
        {
            temp[i][j] = new int[3];
        }
    }
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                temp[i][j][k] = cube[i][j][k];
            }
        }
    }
    temp[2][0][1] = cube[2][1][0];
    temp[2][1][0] = cube[2][2][1];
    temp[2][2][1] = cube[2][1][2];
    temp[2][1][2] = cube[2][0][1];
    temp[2][0][0] = cube[2][2][0];
    temp[2][2][0] = cube[2][2][2];
    temp[2][2][2] = cube[2][0][2];
    temp[2][0][2] = cube[2][0][0];
    temp[0][0][0] = cube[3][0][0];
    temp[0][1][0] = cube[3][1][0];
    temp[0][2][0] = cube[3][2][0];
    temp[3][0][0] = cube[5][0][0];
    temp[3][1][0] = cube[5][1][0];
    temp[3][2][0] = cube[5][2][0];
    temp[5][0][0] = cube[1][0][0];
    temp[5][1][0] = cube[1][1][0];
    temp[5][2][0] = cube[1][2][0];
    temp[1][0][0] = cube[0][0][0];
    temp[1][1][0] = cube[0][1][0];
    temp[1][2][0] = cube[0][2][0];
    return temp;
}

int ***lA(int ***cube)
{
    int ***temp;
    temp = new int **[6];
    for (int i = 0; i < 6; i++)
    {
        temp[i] = new int *[3];
        for (int j = 0; j < 3; j++)
        {
            temp[i][j] = new int[3];
        }
    }
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                temp[i][j][k] = cube[i][j][k];
            }
        }
    }
    temp[2][0][1] = cube[2][1][2];
    temp[2][1][0] = cube[2][0][1];
    temp[2][2][1] = cube[2][1][0];
    temp[2][1][2] = cube[2][2][1];
    temp[2][0][0] = cube[2][0][2];
    temp[2][2][0] = cube[2][0][0];
    temp[2][2][2] = cube[2][2][0];
    temp[2][0][2] = cube[2][2][2];
    temp[0][0][0] = cube[1][0][0];
    temp[0][1][0] = cube[1][1][0];
    temp[0][2][0] = cube[1][2][0];
    temp[3][0][0] = cube[0][0][0];
    temp[3][1][0] = cube[0][1][0];
    temp[3][2][0] = cube[0][2][0];
    temp[5][0][0] = cube[3][0][0];
    temp[5][1][0] = cube[3][1][0];
    temp[5][2][0] = cube[3][2][0];
    temp[1][0][0] = cube[5][0][0];
    temp[1][1][0] = cube[5][1][0];
    temp[1][2][0] = cube[5][2][0];
    return temp;
}

int ***dC(int ***cube)
{
    int ***temp;
    temp = new int **[6];
    for (int i = 0; i < 6; i++)
    {
        temp[i] = new int *[3];
        for (int j = 0; j < 3; j++)
        {
            temp[i][j] = new int[3];
        }
    }
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                temp[i][j][k] = cube[i][j][k];
            }
        }
    }
    temp[5][0][1] = cube[5][1][0];
    temp[5][1][0] = cube[5][2][1];
    temp[5][2][1] = cube[5][1][2];
    temp[5][1][2] = cube[5][0][1];
    temp[5][0][0] = cube[5][2][0];
    temp[5][2][0] = cube[5][2][2];
    temp[5][2][2] = cube[5][0][2];
    temp[5][0][2] = cube[5][0][0];
    temp[2][0][2] = cube[3][0][2];
    temp[2][1][2] = cube[3][1][2];
    temp[2][2][2] = cube[3][2][2];
    temp[3][0][2] = cube[4][0][2];
    temp[3][1][2] = cube[4][1][2];
    temp[3][2][2] = cube[4][2][2];
    temp[4][0][2] = cube[1][0][2];
    temp[4][1][2] = cube[1][1][2];
    temp[4][2][2] = cube[1][2][2];
    temp[1][0][2] = cube[2][0][2];
    temp[1][1][2] = cube[2][1][2];
    temp[1][2][2] = cube[2][2][2];
    return temp;
}

int ***dA(int ***cube)
{
    int ***temp;
    temp = new int **[6];
    for (int i = 0; i < 6; i++)
    {
        temp[i] = new int *[3];
        for (int j = 0; j < 3; j++)
        {
            temp[i][j] = new int[3];
        }
    }
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                temp[i][j][k] = cube[i][j][k];
            }
        }
    }
    temp[5][0][1] = cube[5][1][2];
    temp[5][1][0] = cube[5][0][1];
    temp[5][2][1] = cube[5][1][0];
    temp[5][1][2] = cube[5][2][1];
    temp[5][0][0] = cube[5][0][2];
    temp[5][2][0] = cube[5][0][0];
    temp[5][2][2] = cube[5][2][0];
    temp[5][0][2] = cube[5][2][2];
    temp[2][0][2] = cube[1][0][2];
    temp[2][1][2] = cube[1][1][2];
    temp[2][2][2] = cube[1][2][2];
    temp[3][0][2] = cube[2][0][2];
    temp[3][1][2] = cube[2][1][2];
    temp[3][2][2] = cube[2][2][2];
    temp[4][0][2] = cube[3][0][2];
    temp[4][1][2] = cube[3][1][2];
    temp[4][2][2] = cube[3][2][2];
    temp[1][0][2] = cube[4][0][2];
    temp[1][1][2] = cube[4][1][2];
    temp[1][2][2] = cube[4][2][2];
    return temp;
}

int ***fC(int ***cube)
{
    int ***temp;
    temp = new int **[6];
    for (int i = 0; i < 6; i++)
    {
        temp[i] = new int *[3];
        for (int j = 0; j < 3; j++)
        {
            temp[i][j] = new int[3];
        }
    }
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                temp[i][j][k] = cube[i][j][k];
            }
        }
    }
    temp[1][0][1] = cube[1][1][2];
    temp[1][1][0] = cube[1][0][1];
    temp[1][2][1] = cube[1][1][0];
    temp[1][1][2] = cube[1][2][1];
    temp[1][0][0] = cube[1][0][2];
    temp[1][2][0] = cube[1][0][0];
    temp[1][2][2] = cube[1][2][0];
    temp[1][0][2] = cube[1][2][2];
    temp[0][2][0] = cube[2][2][2];
    temp[0][2][1] = cube[2][1][2];
    temp[0][2][2] = cube[2][0][2];
    temp[4][0][0] = cube[0][2][2];
    temp[4][1][0] = cube[0][2][1];
    temp[4][2][0] = cube[0][2][0];
    temp[5][0][2] = cube[4][0][0];
    temp[5][0][1] = cube[4][1][0];
    temp[5][0][0] = cube[4][2][0];
    temp[2][0][2] = cube[5][0][0];
    temp[2][1][2] = cube[5][0][1];
    temp[2][2][2] = cube[5][0][2];
    return temp;
}

int ***fA(int ***cube)
{
    int ***temp;
    temp = new int **[6];
    for (int i = 0; i < 6; i++)
    {
        temp[i] = new int *[3];
        for (int j = 0; j < 3; j++)
        {
            temp[i][j] = new int[3];
        }
    }
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                temp[i][j][k] = cube[i][j][k];
            }
        }
    }
    temp[1][0][1] = cube[1][1][0];
    temp[1][1][0] = cube[1][2][1];
    temp[1][2][1] = cube[1][1][2];
    temp[1][1][2] = cube[1][0][1];
    temp[1][0][0] = cube[1][2][0];
    temp[1][2][0] = cube[1][2][2];
    temp[1][2][2] = cube[1][0][2];
    temp[1][0][2] = cube[1][0][0];
    temp[0][2][2] = cube[4][0][0];
    temp[0][2][1] = cube[4][1][0];
    temp[0][2][0] = cube[4][2][0];
    temp[2][0][2] = cube[0][2][2];
    temp[2][1][2] = cube[0][2][1];
    temp[2][2][2] = cube[0][2][0];
    temp[5][0][0] = cube[2][0][2];
    temp[5][0][1] = cube[2][1][2];
    temp[5][0][2] = cube[2][2][2];
    temp[4][2][0] = cube[5][0][2];
    temp[4][1][0] = cube[5][0][1];
    temp[4][0][0] = cube[5][0][0];
    return temp;
}

int ***bC(int ***cube)
{
    int ***temp;
    temp = new int **[6];
    for (int i = 0; i < 6; i++)
    {
        temp[i] = new int *[3];
        for (int j = 0; j < 3; j++)
        {
            temp[i][j] = new int[3];
        }
    }
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                temp[i][j][k] = cube[i][j][k];
            }
        }
    }
    temp[3][0][1] = cube[3][1][0];
    temp[3][1][0] = cube[3][2][1];
    temp[3][2][1] = cube[3][1][2];
    temp[3][1][2] = cube[3][0][1];
    temp[3][0][0] = cube[3][2][0];
    temp[3][2][0] = cube[3][2][2];
    temp[3][2][2] = cube[3][0][2];
    temp[3][0][2] = cube[3][0][0];
    temp[0][0][0] = cube[4][2][2];
    temp[0][0][1] = cube[4][1][2];
    temp[0][0][2] = cube[4][0][2];
    temp[2][2][0] = cube[0][0][0];
    temp[2][1][0] = cube[0][0][1];
    temp[2][0][0] = cube[0][0][2];
    temp[5][2][2] = cube[2][0][0];
    temp[5][2][1] = cube[2][1][0];
    temp[5][2][0] = cube[2][2][0];
    temp[4][0][2] = cube[5][2][2];
    temp[4][1][2] = cube[5][2][1];
    temp[4][2][2] = cube[5][2][0];
    return temp;
}

int ***bA(int ***cube)
{
    int ***temp;
    temp = new int **[6];
    for (int i = 0; i < 6; i++)
    {
        temp[i] = new int *[3];
        for (int j = 0; j < 3; j++)
        {
            temp[i][j] = new int[3];
        }
    }
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                temp[i][j][k] = cube[i][j][k];
            }
        }
    }
    temp[3][0][1] = cube[3][1][2];
    temp[3][1][0] = cube[3][0][1];
    temp[3][2][1] = cube[3][1][0];
    temp[3][1][2] = cube[3][2][1];
    temp[3][0][0] = cube[3][0][2];
    temp[3][2][0] = cube[3][0][0];
    temp[3][2][2] = cube[3][2][0];
    temp[3][0][2] = cube[3][2][2];
    temp[0][0][2] = cube[2][0][0];
    temp[0][0][1] = cube[2][1][0];
    temp[0][0][0] = cube[2][2][0];
    temp[4][0][2] = cube[0][0][2];
    temp[4][1][2] = cube[0][0][1];
    temp[4][2][2] = cube[0][0][0];
    temp[5][2][0] = cube[4][2][2];
    temp[5][2][1] = cube[4][1][2];
    temp[5][2][2] = cube[4][0][2];
    temp[2][2][0] = cube[5][2][2];
    temp[2][1][0] = cube[5][2][1];
    temp[2][0][0] = cube[5][2][0];
    return temp;
}

struct Vec3D
{
    int x, y, z;
    bool operator<(const Vec3D &other) const
    {
        if (x != other.x)
            return x < other.x;
        if (y != other.y)
            return y < other.y;
        return z < other.z;
    }
    bool operator==(const Vec3D &other) const
    {
        return x == other.x && y == other.y && z == other.z;
    }
};

struct CubieDefinition
{
    string name;
    bool is_corner;
    set<int> colors;
    Vec3D pos;
    vector<tuple<int, int, int> > idxs;
};

vector<CubieDefinition> cubies;
map<set<int>, Vec3D> colorToPos;

int manhattan_distance(const Vec3D &c1, const Vec3D &c2)
{
    return abs(c1.x - c2.x) + abs(c1.y - c2.y) + abs(c1.z - c2.z);
}

set<int> get_colors(int ***state, const vector<tuple<int, int, int> > &idxs)
{
    set<int> colors;
    for (const auto &loc : idxs)
    {
        colors.insert(state[get<0>(loc)][get<1>(loc)][get<2>(loc)]);
    }
    return colors;
}

void init_cubies(int ***solved)
{
    cubies.clear();
    colorToPos.clear();

    CubieDefinition defs[] = {
        {"UFR", true, {}, {1, 1, 1}, {{0, 2, 2}, {1, 0, 2}, {4, 0, 0}}},
        {"UFL", true, {}, {-1, 1, 1}, {{0, 2, 0}, {1, 0, 0}, {2, 0, 2}}},
        {"UBL", true, {}, {-1, 1, -1}, {{0, 0, 0}, {2, 0, 0}, {3, 0, 2}}},
        {"UBR", true, {}, {1, 1, -1}, {{0, 0, 2}, {3, 0, 0}, {4, 0, 2}}},
        {"DFR", true, {}, {1, -1, 1}, {{5, 0, 2}, {1, 2, 2}, {4, 2, 0}}},
        {"DFL", true, {}, {-1, -1, 1}, {{5, 0, 0}, {1, 2, 0}, {2, 2, 2}}},
        {"DBL", true, {}, {-1, -1, -1}, {{5, 2, 0}, {2, 2, 0}, {3, 2, 2}}},
        {"DBR", true, {}, {1, -1, -1}, {{5, 2, 2}, {3, 2, 0}, {4, 2, 2}}},
        {"UF", false, {}, {0, 1, 1}, {{0, 2, 1}, {1, 0, 1}}},
        {"UL", false, {}, {-1, 1, 0}, {{0, 1, 0}, {2, 0, 1}}},
        {"UB", false, {}, {0, 1, -1}, {{0, 0, 1}, {3, 0, 1}}},
        {"UR", false, {}, {1, 1, 0}, {{0, 1, 2}, {4, 0, 1}}},
        {"FL", false, {}, {-1, 0, 1}, {{1, 1, 0}, {2, 1, 2}}},
        {"FR", false, {}, {1, 0, 1}, {{1, 1, 2}, {4, 1, 0}}},
        {"BL", false, {}, {-1, 0, -1}, {{3, 1, 2}, {2, 1, 0}}},
        {"BR", false, {}, {1, 0, -1}, {{3, 1, 0}, {4, 1, 2}}},
        {"DF", false, {}, {0, -1, 1}, {{5, 0, 1}, {1, 2, 1}}},
        {"DL", false, {}, {-1, -1, 0}, {{5, 1, 0}, {2, 2, 1}}},
        {"DB", false, {}, {0, -1, -1}, {{5, 2, 1}, {3, 2, 1}}},
        {"DR", false, {}, {1, -1, 0}, {{5, 1, 2}, {4, 2, 1}}},
    };

    for (CubieDefinition &def : defs)
    {
        def.colors = get_colors(solved, def.idxs);
        cubies.push_back(def);
        colorToPos[def.colors] = def.pos;
    }
    cout << "Manhattan heuristic cubie definitions initialized: " << cubies.size() << " cubies." << endl;
}

int heuristic(int ***state, int ***goal)
{
    if (cubies.empty())
    {
        init_cubies(goal);
    }

    long long corner_dist = 0;
    long long edge_dist = 0;

    for (const auto &cubie : cubies)
    {
        set<int> curr_colors = get_colors(state, cubie.idxs);
        auto it = colorToPos.find(curr_colors);
        if (it != colorToPos.end())
        {
            Vec3D home = it->second;
            Vec3D curr = cubie.pos;
            int dist = manhattan_distance(curr, home);
            if (cubie.is_corner)
            {
                corner_dist += dist;
            }
            else
            {
                edge_dist += dist;
            }
        }
    }
    return max(corner_dist, edge_dist);
}

int ***getCube()
{
    int ***temp;
    temp = new int **[6];
    for (int i = 0; i < 6; i++)
    {
        temp[i] = new int *[3];
        for (int j = 0; j < 3; j++)
        {
            temp[i][j] = new int[3];
        }
    }
    return temp;
}

class node
{
public:
    int ***curr;
    int prevMove;

    node()
    {
        prevMove = 0;
        curr = getCube();
    }

    ~node()
    {
        if (curr != nullptr)
        {
            for (int i = 0; i < 6; ++i)
            {
                if (curr[i] != nullptr)
                {
                    for (int j = 0; j < 3; ++j)
                    {
                        delete[] curr[i][j];
                    }
                    delete[] curr[i];
                }
            }
            delete[] curr;
            curr = nullptr;
        }
    }

    void setCS(int ***c)
    {
        for (int i = 0; i < 6; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                for (int k = 0; k < 3; k++)
                {
                    curr[i][j][k] = c[i][j][k];
                }
            }
        }
    }
};

typedef int ***(*MF)(int ***);
MF mf[] = {
    lC, lA, rC, rA,
    uC, uA, dC, dA,
    fC, fA, bC, bA};

int ida_star_step(node *cur, int g, int limit, int ***goal, stack<int> &path, stack<int> &sol)
{
    int h = heuristic(cur->curr, goal);
    int f = g + h;

    if (f > limit)
    {
        return f; // Exceeded current threshold, return new candidate threshold
    }

    if (heuristic(cur->curr, goal) == 0)
    {
        sol = path; // Solution found, save the path
        return 0;
    }

    int min_cost = numeric_limits<int>::max();

    // Try each possible move
    for (int i = 0; i < 12; ++i)
    {
        int move = i + 1;

        if (cur->prevMove != 0)
        {
            bool is_inverse = false;
            if ((cur->prevMove % 2 == 1 && move == cur->prevMove + 1) || (cur->prevMove % 2 == 0 && move == cur->prevMove - 1))
                is_inverse = true;
            if (is_inverse)
                continue;
        }

        node *n = new node();
        n->curr = mf[i](cur->curr);
        n->prevMove = move;

        path.push(move);
        int r = ida_star_step(n, g + 1, limit, goal, path, sol); // Recursive DFS exploration
        path.pop();

        delete n;

        if (r == 0)
            return 0;
        if (r < min_cost)
            min_cost = r;
    }
    return min_cost; // Return minimum cost for next threshold
}

void printSolution(stack<int> s)
{
    if (s.empty())
    {
        cout << "No moves in solution (already solved or no solution found)." << endl;
        return;
    }
    vector<int> moves_vec;
    while (!s.empty())
    {
        moves_vec.push_back(s.top());
        s.pop();
    }
    for (int i = moves_vec.size() - 1; i >= 0; --i)
    {
        int p = moves_vec[i];
        if (p == 1)
            cout << "move Left ClockWise" << endl;
        else if (p == 2)
            cout << "move Left AntiClockWise" << endl;
        else if (p == 3)
            cout << "move Right ClockWise" << endl;
        else if (p == 4)
            cout << "move Right AntiClockWise" << endl;
        else if (p == 5)
            cout << "move Up ClockWise" << endl;
        else if (p == 6)
            cout << "move Up AntiClockWise" << endl;
        else if (p == 7)
            cout << "move Down ClockWise" << endl;
        else if (p == 8)
            cout << "move Down AntiClockWise" << endl;
        else if (p == 9)
            cout << "move Front ClockWise" << endl;
        else if (p == 10)
            cout << "move Front AntiClockWise" << endl;
        else if (p == 11)
            cout << "move Back ClockWise" << endl;
        else if (p == 12)
            cout << "move Back AntiClockWise" << endl;
        else
            cout << "Unknown move: " << p << endl;
    }
    cout << endl;
}

int main(int argc, char *argv[])
{
    int ***start = getCube();
    int ***goal = getCube();

    takeInput(start, goal, argc, argv);

    init_cubies(goal);

    cout << "Initial state Manhattan heuristic: " << heuristic(start, goal) << endl;

    int limit = heuristic(start, goal);
    stack<int> path;
    stack<int> sol;

    const int MAX_DEPTH = 25;

    while (true)
    {
        node *root = new node();
        root->setCS(start);
        root->prevMove = 0;

        cout << "Searching with cost limit (f_max): " << limit << endl;

        path = stack<int>();
        int next = ida_star_step(root, 0, limit, goal, path, sol);

        delete root;

        if (next == 0)
        {
            cout << "Found!" << endl;
            cout << "The depth (number of moves) is: " << sol.size() << endl;
            printSolution(sol);
            break;
        }

        if (next == numeric_limits<int>::max())
        {
            cout << "Not Found (exhausted search space or no solution within limits). Current cost_limit: " << limit << endl;
            break;
        }

        limit = next; // Increase the threshold for next iteration

        if (limit > MAX_DEPTH * 2)
        {
            cout << "Search limit exceeded (cost_limit " << limit << " > MAX_DEPTH " << MAX_DEPTH << "). Stopping." << endl;
            break;
        }
    }

    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            delete[] start[i][j];
            delete[] goal[i][j];
        }
        delete[] start[i];
        delete[] goal[i];
    }
    delete[] start;
    delete[] goal;

    return 0;
}