#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <string>
#include <vector>
#include <stack>
#include <limits>
#include <algorithm>
#include <map>
#include <set>
#include <sstream>
#include <numeric>

using namespace std;
using namespace emscripten;

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

struct IdaNode
{
    int ***cube_state_ptr;
    int prev_move_val;

    IdaNode() : prev_move_val(0)
    {
        cube_state_ptr = new int **[6];
        for (int i = 0; i < 6; i++)
        {
            cube_state_ptr[i] = new int *[3];
            for (int j = 0; j < 3; j++)
            {
                cube_state_ptr[i][j] = new int[3]();
            }
        }
    }

    ~IdaNode()
    {
        if (cube_state_ptr != nullptr)
        {
            for (int i = 0; i < 6; ++i)
            {
                if (cube_state_ptr[i] != nullptr)
                {
                    for (int j = 0; j < 3; ++j)
                    {
                        delete[] cube_state_ptr[i][j];
                    }
                    delete[] cube_state_ptr[i];
                }
            }
            delete[] cube_state_ptr;
            cube_state_ptr = nullptr;
        }
    }

    void set_cube_state(int ***const source_cube)
    {
        for (int i = 0; i < 6; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                for (int k = 0; k < 3; k++)
                {
                    cube_state_ptr[i][j][k] = source_cube[i][j][k];
                }
            }
        }
    }
};

typedef int ***(*MoveFunctionPtr)(int ***);

class RubiksCubeEngine
{
private:
    int ***current_cube_state;
    int ***goal_cube_state;

    static vector<CubieDefinition> static_cubies;
    static map<set<int>, Vec3D> static_color_to_pos;
    static bool static_cubies_initialized;

    static MoveFunctionPtr move_functions[12];

    int ***allocate_cube_memory()
    {
        int ***cube = new int **[6];
        for (int i = 0; i < 6; i++)
        {
            cube[i] = new int *[3];
            for (int j = 0; j < 3; j++)
            {
                cube[i][j] = new int[3]();
            }
        }
        return cube;
    }

    void delete_cube_memory(int ***&cube)
    {
        if (cube != nullptr)
        {
            for (int i = 0; i < 6; ++i)
            {
                if (cube[i] != nullptr)
                {
                    for (int j = 0; j < 3; ++j)
                    {
                        delete[] cube[i][j];
                    }
                    delete[] cube[i];
                }
            }
            delete[] cube;
            cube = nullptr;
        }
    }

    void copy_cube_data(int ***dest, int ***const src)
    {
        for (int i = 0; i < 6; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                for (int k = 0; k < 3; k++)
                {
                    dest[i][j][k] = src[i][j][k];
                }
            }
        }
    }

    void flat_to_3d(const vector<int> &flat_state, int ***target_cube)
    {
        if (flat_state.size() != 54)
            return;
        int idx = 0;
        for (int i = 0; i < 6; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                for (int k = 0; k < 3; k++)
                {
                    target_cube[i][j][k] = flat_state[idx++];
                }
            }
        }
    }

    static int ***getCube()
    {
        int ***temp;
        temp = new int **[6];
        for (int i = 0; i < 6; i++)
        {
            temp[i] = new int *[3];
            for (int j = 0; j < 3; j++)
            {
                temp[i][j] = new int[3];
                for (int k = 0; k < 3; k++)
                {
                    temp[i][j][k] = 0;
                }
            }
        }
        return temp;
    }

    static int ***rot_lC(int ***cube)
    {
        int ***temp = getCube();
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

    static int ***rot_lA(int ***cube)
    {
        int ***temp = getCube();
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

    static int ***rot_rC(int ***cube)
    {
        int ***temp = getCube();
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

    static int ***rot_rA(int ***cube)
    {
        int ***temp = getCube();
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

    static int ***rot_uC(int ***cube)
    {
        int ***temp = getCube();
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

    static int ***rot_uA(int ***cube)
    {
        int ***temp = getCube();
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

    static int ***rot_dC(int ***cube)
    {
        int ***temp = getCube();
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

    static int ***rot_dA(int ***cube)
    {
        int ***temp = getCube();
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

    static int ***rot_fC(int ***cube)
    {
        int ***temp = getCube();
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

    static int ***rot_fA(int ***cube)
    {
        int ***temp = getCube();
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

    static int ***rot_bC(int ***cube)
    {
        int ***temp = getCube();
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

    static int ***rot_bA(int ***cube)
    {
        int ***temp = getCube();
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

    static set<int> get_cubie_colors(int ***state, const vector<tuple<int, int, int> > &idxs)
    {
        set<int> colors;
        for (const auto &loc : idxs)
        {
            colors.insert(state[get<0>(loc)][get<1>(loc)][get<2>(loc)]);
        }
        return colors;
    }

    static void initialize_cubie_definitions(int ***solved_state)
    {
        static_cubies.clear();
        static_color_to_pos.clear();

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
            def.colors = get_cubie_colors(solved_state, def.idxs);
            static_cubies.push_back(def);
            static_color_to_pos[def.colors] = def.pos;
        }
        static_cubies_initialized = true;
    }

    static int manhattan_distance(const Vec3D &c1, const Vec3D &c2)
    {
        return abs(c1.x - c2.x) + abs(c1.y - c2.y) + abs(c1.z - c2.z);
    }

    int calculate_heuristic(int ***current_state_h)
    {
        if (!static_cubies_initialized)
        {
            initialize_cubie_definitions(goal_cube_state);
        }

        long long corner_dist = 0;
        long long edge_dist = 0;

        for (const auto &cubie_def : static_cubies)
        {
            set<int> current_colors_at_slot = get_cubie_colors(current_state_h, cubie_def.idxs);
            auto it_goal_pos = static_color_to_pos.find(current_colors_at_slot);

            if (it_goal_pos != static_color_to_pos.end())
            {
                Vec3D goal_pos_for_this_piece = it_goal_pos->second;
                Vec3D current_slot_def_pos = cubie_def.pos;

                int dist = manhattan_distance(current_slot_def_pos, goal_pos_for_this_piece);

                if (cubie_def.is_corner)
                {
                    corner_dist += dist;
                }
                else
                {
                    edge_dist += dist;
                }
            }
        }
        return max(corner_dist / 4, edge_dist / 4);
    }

    int ida_star_search_step(IdaNode *current_node, int g_cost, int cost_limit, stack<int> &path_stack, stack<int> &solution_stack)
    {
        int h_cost = calculate_heuristic(current_node->cube_state_ptr);
        int f_cost = g_cost + h_cost;

        if (f_cost > cost_limit)
        {
            return f_cost;
        }

        if (h_cost == 0)
        {
            solution_stack = path_stack;
            return 0;
        }

        int min_total_cost = numeric_limits<int>::max();

        for (int i = 0; i < 12; ++i)
        {
            int move_id = i + 1;

            if (current_node->prev_move_val != 0)
            {
                bool is_inverse_move = false;
                if ((current_node->prev_move_val % 2 == 1 && move_id == current_node->prev_move_val + 1) ||
                    (current_node->prev_move_val % 2 == 0 && move_id == current_node->prev_move_val - 1))
                {
                    is_inverse_move = true;
                }
                if (is_inverse_move)
                    continue;
                if ((current_node->prev_move_val - 1) / 2 == (move_id - 1) / 2)
                    continue;
            }

            IdaNode *next_node = new IdaNode();

            int ***temp_moved_cube = move_functions[i](current_node->cube_state_ptr);

            delete_cube_memory(next_node->cube_state_ptr);
            next_node->cube_state_ptr = temp_moved_cube;
            next_node->prev_move_val = move_id;

            path_stack.push(move_id);
            int result_cost = ida_star_search_step(next_node, g_cost + 1, cost_limit, path_stack, solution_stack);
            path_stack.pop();

            delete next_node;

            if (result_cost == 0)
                return 0;
            if (result_cost < min_total_cost)
            {
                min_total_cost = result_cost;
            }
        }
        return min_total_cost;
    }

    string format_solution(stack<int> s)
    {
        if (s.empty())
            return "Already solved or no solution found.";

        stringstream ss;
        vector<int> moves_vec;
        while (!s.empty())
        {
            moves_vec.push_back(s.top());
            s.pop();
        }
        reverse(moves_vec.begin(), moves_vec.end());

        for (size_t i = 0; i < moves_vec.size(); ++i)
        {
            int p = moves_vec[i];
            if (p == 1)
                ss << "L ";
            else if (p == 2)
                ss << "L' ";
            else if (p == 3)
                ss << "R ";
            else if (p == 4)
                ss << "R' ";
            else if (p == 5)
                ss << "U ";
            else if (p == 6)
                ss << "U' ";
            else if (p == 7)
                ss << "D ";
            else if (p == 8)
                ss << "D' ";
            else if (p == 9)
                ss << "F ";
            else if (p == 10)
                ss << "F' ";
            else if (p == 11)
                ss << "B ";
            else if (p == 12)
                ss << "B' ";
            else
                ss << "? ";
        }
        string result = ss.str();
        if (!result.empty())
            result.pop_back();
        return result;
    }

public:
    RubiksCubeEngine() : current_cube_state(nullptr), goal_cube_state(nullptr)
    {
        current_cube_state = allocate_cube_memory();
        goal_cube_state = allocate_cube_memory();

        vector<int> default_flat_goal(54);
        for (int face = 0; face < 6; ++face)
        {
            for (int tile = 0; tile < 9; ++tile)
            {
                default_flat_goal[face * 9 + tile] = face;
            }
        }
        flat_to_3d(default_flat_goal, goal_cube_state);

        if (!static_cubies_initialized)
        {
            initialize_cubie_definitions(goal_cube_state);
        }

        move_functions[0] = &RubiksCubeEngine::rot_lC;
        move_functions[1] = &RubiksCubeEngine::rot_lA;
        move_functions[2] = &RubiksCubeEngine::rot_rC;
        move_functions[3] = &RubiksCubeEngine::rot_rA;
        move_functions[4] = &RubiksCubeEngine::rot_uC;
        move_functions[5] = &RubiksCubeEngine::rot_uA;
        move_functions[6] = &RubiksCubeEngine::rot_dC;
        move_functions[7] = &RubiksCubeEngine::rot_dA;
        move_functions[8] = &RubiksCubeEngine::rot_fC;
        move_functions[9] = &RubiksCubeEngine::rot_fA;
        move_functions[10] = &RubiksCubeEngine::rot_bC;
        move_functions[11] = &RubiksCubeEngine::rot_bA;
    }

    ~RubiksCubeEngine()
    {
        delete_cube_memory(current_cube_state);
        delete_cube_memory(goal_cube_state);
    }

    void setStartState(const vector<int> &flat_state)
    {
        if (flat_state.size() != 54)
        {
            return;
        }
        flat_to_3d(flat_state, current_cube_state);
    }

    void setGoalState(const vector<int> &flat_goal_state)
    {
        if (flat_goal_state.size() != 54)
        {
            return;
        }
        flat_to_3d(flat_goal_state, goal_cube_state);
        initialize_cubie_definitions(goal_cube_state);
    }

    string solve()
    {
        if (goal_cube_state == nullptr || current_cube_state == nullptr)
        {
            return "Error: Solver not initialized properly (goal or start state missing).";
        }
        if (!static_cubies_initialized)
        {
            initialize_cubie_definitions(goal_cube_state);
        }

        int initial_heuristic = calculate_heuristic(current_cube_state);
        if (initial_heuristic == 0)
        {
            bool truly_solved = true;
            for (int f = 0; f < 6; ++f)
                for (int r = 0; r < 3; ++r)
                    for (int c = 0; c < 3; ++c)
                        if (current_cube_state[f][r][c] != goal_cube_state[f][r][c])
                            truly_solved = false;
            if (truly_solved)
                return "Already solved.";
        }

        int cost_limit = initial_heuristic;
        stack<int> path_stack;
        stack<int> solution_stack;

        const int MAX_ITERATIONS = 30;
        int iteration_count = 0;

        while (iteration_count < MAX_ITERATIONS)
        {
            IdaNode *root_node = new IdaNode();
            root_node->set_cube_state(current_cube_state);
            root_node->prev_move_val = 0;

            path_stack = stack<int>();
            int next_cost_limit = ida_star_search_step(root_node, 0, cost_limit, path_stack, solution_stack);

            delete root_node;

            if (next_cost_limit == 0)
            {
                return format_solution(solution_stack);
            }

            if (next_cost_limit == numeric_limits<int>::max())
            {
                return "No solution found (exhausted search space or no solution within limits).";
            }

            cost_limit = next_cost_limit;
            iteration_count++;

            if (cost_limit > 22 && calculate_heuristic(current_cube_state) > 0)
            {
                return "Search limit exceeded (cost too high). Might be unsolvable or too complex.";
            }
        }
        return "Error: Solver loop iterations exceeded.";
    }

    string isSolved(const vector<int> &flat_state)
    {
        if (flat_state.size() != 54)
            return "false (invalid board size)";
        if (!goal_cube_state)
            return "false (goal not set)";

        int ***temp_cube = allocate_cube_memory();
        flat_to_3d(flat_state, temp_cube);

        bool structure_match = true;
        for (int f = 0; f < 6; ++f)
        {
            for (int r = 0; r < 3; ++r)
            {
                for (int c = 0; c < 3; ++c)
                {
                    if (temp_cube[f][r][c] != goal_cube_state[f][r][c])
                    {
                        structure_match = false;
                        break;
                    }
                }
                if (!structure_match)
                    break;
            }
            if (!structure_match)
                break;
        }
        delete_cube_memory(temp_cube);
        return structure_match ? "true" : "false";
    }
};

vector<CubieDefinition> RubiksCubeEngine::static_cubies;
map<set<int>, Vec3D> RubiksCubeEngine::static_color_to_pos;
bool RubiksCubeEngine::static_cubies_initialized = false;
MoveFunctionPtr RubiksCubeEngine::move_functions[12];

EMSCRIPTEN_BINDINGS(rubiks_module)
{
    register_vector<int>("IntVector");

    class_<RubiksCubeEngine>("RubiksCubeEngine")
        .constructor<>()
        .function("setStartState", &RubiksCubeEngine::setStartState)
        .function("setGoalState", &RubiksCubeEngine::setGoalState)
        .function("solve", &RubiksCubeEngine::solve)
        .function("isSolved", &RubiksCubeEngine::isSolved);
}