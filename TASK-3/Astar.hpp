
#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <limits>
#include <functional>
#include <cstdlib>
#include "cell.hpp"

using namespace std;

// 曼哈顿距离
int hm(cell current, cell G) {
    return abs(current.row - G.row)+ abs(current.col - G.col);
}

// A* 节点
struct Node {
    cell pos;
    int g;
    int f;
    //后续在open中进行greater比较
    bool operator>(const Node& other) const {
        return f > other.f;
    }
};

// A* 求最小通行代价
int Astar(const vector<vector<string>>& map,cell S,cell G) {
    if (map.empty())
     return -1;

    if (map[S.row][S.col] == "#" ||map[G.row][G.col] == "#") {
        return -1;
    }

    int dr[4] = {-1, 0, 1, 0};
    int dc[4] = {0, 1, 0, -1};

    const int INF = numeric_limits<int>::max();

    // 记录起点到每个格子的最小实际代价
    vector<vector<int>> best_g;
    //在这里除了起点的g值，其他点均为INF
  for (const auto& row : map) {
        best_g.emplace_back(row.size(), INF);
    }

    priority_queue<Node, vector<Node>, greater<Node>> open;//创建优先队列，让f最小的现出队列
    //priority_queue<数据类型, 底层容器, 比较方式>
    // 初始化起点
    best_g[S.row][S.col] = 0;

    open.push({
        S,
        0,
        hm(S, G)
    });
while (!open.empty()) {
        // 取出 f 最小的节点
        Node current = open.top();//当一个节点从优先队列中取出时，其 g 值已经确定为最优值，一致的启发函数
        open.pop();
        cell pos = current.pos;
        // 跳过已经过时的队列记录
    if (current.g != best_g[pos.row][pos.col]) {
            continue;
        }
        // 到达终点，返回最小代价
        if (pos.row == G.row &&pos.col == G.col) {
            return current.g;
        }
        // 遍历上下左右四个方向
    for (int i = 0; i < 4; i++) {
          int nr = pos.row + dr[i];
            int nc = pos.col + dc[i];
            // 判断边界
            if (pos.row+dr[i]<0||pos.row+dr[i]>=map.size()||pos.col+dc[i]<0||pos.col+dc[i]>=map[pos.row+dr[i]].size()) {
                continue;
            }
            // 判断障碍物
            if (map[pos.row + dr[i]][pos.col + dc[i]] == "#") {
                continue;
            }
            // 正常的代价为1
            int step_cost = 1;
            if (map[nr][nc].size() == 1 &&map[nr][nc][0] >= '2' &&map[nr][nc][0] <= '9') {
                step_cost = map[nr][nc][0] - '0';
            }

            // 计算新的实际代价
            int new_g = current.g + step_cost;

            // 只有找到更小代价才更新
            if (new_g < best_g[nr][nc]) {
                best_g[nr][nc] = new_g;//通过这个操作保证格子的g一定为最小
                cell next{nr, nc};

                int h = hm(next, G);
                int f = new_g + h;

                open.push({
                    next,
                    new_g,//最小代价
                    f
                });
            }
        }
    }

    // 队列为空，说明终点不可达
    return -1;
}
