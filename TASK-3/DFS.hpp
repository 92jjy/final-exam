#include<iostream>
#include<vector>
#include<string>
#include "cell.hpp"

using namespace std;
//题目要求表明参数不需要加入G,到达终点的判断由A*来做
int DFS(const vector<vector<string>>&map,cell S,vector<vector<bool>>&visited){
int dr[4] = {-1, 0, 1, 0};
int dc[4] = {0, 1, 0, -1};
visited[S.row][S.col]=true;
int count=1;//这里每次用到count均会为一，下面要用累加形式求和
for(int i=0;i<4;i++){
    //这里的边界判断我一开始没有加等号，注意
    if(S.row+dr[i]<0||S.row+dr[i]>=map.size()||S.col+dc[i]<0||S.col+dc[i]>=map[S.row+dr[i]].size()){
        continue;
    }
    if(map[S.row+dr[i]][S.col+dc[i]]!="#"&&!visited[S.row+dr[i]][S.col+dc[i]]){
        visited[S.row+dr[i]][S.col+dc[i]]=true;
       cell next{S.row+dr[i],S.col+dc[i]};
        count+=DFS(map,next,visited);
    }
}
return count;
}

int regions(const vector<vector<string>>&map,vector<vector<bool>>&visited){
     int region = 0;
// 遍历整张地图
for (int row = 0; row < map.size(); row++) {
    for (int col = 0; col < map[row].size(); col++) {
        // 代表找到一个新的区域
        if (!visited[row][col]&&map[row][col]!="#") {
            cell next={row,col};
            DFS(map,next,visited);//调用DFS填补空白区域
            region++;
        }
    }
}
return region;
}

