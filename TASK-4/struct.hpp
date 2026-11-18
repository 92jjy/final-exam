#pragma once
//此文件用于存放三个结构体
#include<iostream>
#include<string>
#include <type_traits>

using namespace std;

struct CameraFrame { int w, h; const unsigned char* buf; };
struct RosImage    { int w, h, step; const unsigned char* data; };  // step 为每行字节数，step >= w
struct VideoFrame  { int w, h; const unsigned char* yuv; };         // 只使用前 w*h 字节（Y 平面）

