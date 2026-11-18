#include<iostream>
#include<string>
#include <type_traits>
#include"struct.hpp"
using namespace std;


namespace tmpl {
// Frame 为 CameraFrame、RosImage、VideoFrame 之一，直接使用上面的原始结构体，不需要再包装成类
template <typename Frame>
std::uint64_t preprocess(const Frame& f){
    uint64_t sum=0;
    int w=f.w;
    int h=f.h;
    for(int y=0;y<h;y++){
        for(int x=0;x<w;x++){
            if constexpr(is_same_v<Frame,CameraFrame>)//if constexpr 即可判断
            {
            sum += f.buf[y * w + x];
            }
            // ros
            else if constexpr(is_same_v<Frame,RosImage>) {

                sum += f.data[y * f.step + x];
            }
            // 视频图像
            else if constexpr (is_same_v<Frame, VideoFrame>) {
                int value = f.yuv[y * w + x] - 16;
                if (value < 0) {
                    value = 0;
                }
                sum += value;
        }
    }
    
}
return sum;
}
}
 