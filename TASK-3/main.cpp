#include <iostream>
#include <string>
#include <vector>

#include "DFS.hpp"
#include "Astar.hpp"
using namespace std;
//此函数用于接入map实例,map地图以vector数组的形式存储
void solvecase() {
    int R, C;
    if (!(std::cin >> R >> C)) {
        return;
    }

    vector<vector<string>> map(R, vector<string>(C));
    cell S{}, G{};
    // 读取地图时同时记录 S 和 G 的坐标。
    for (int row = 0; row < R; ++row) {
        string line;
        std::cin >> line;
        for (int col = 0; col < C; ++col) {
            map[row][col] = line[col];
            if (line[col] == 'S') {
                S = {row, col};
            } else if (line[col] == 'G') {
                G = {row, col};
            }
        }
    }
    vector<vector<bool>> visited(R, vector<bool>(C, false));
    // DFS 先判断终点是否可达，可达时再用 A* 求最小通行代价。
    const auto result = DFS(map, S, visited);//使用DFS算法算出能走到的格子数与整张地图中互不联通的可通行区域，采取递归出stack的方式
    const bool goal_reachable = visited[G.row][G.col];
    const int result2 = 1 + regions(map, visited);
    //astarAPI
    const int cost = goal_reachable ? Astar(map, S, G) : -1;//使用A*算法算出曼哈顿距离
    std::cout << "reachable_cells: " << result << '\n'
              << "regions: " << result2 << '\n'
              << "cost: " << cost << '\n';
}

//主函数，产出最终结果
int main() {
    solvecase();
}
