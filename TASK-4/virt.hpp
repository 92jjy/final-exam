#include<iostream>
#include<string>
#include"struct.hpp"
using namespace std;



namespace virt {
class ImageSource {
public:
    virtual ~ImageSource() = default;
    virtual int width() const = 0;
    virtual int height() const = 0;
    virtual unsigned char at(int x, int y) const = 0;
};

class CameraSource : public ImageSource {
private:
CameraFrame Frame;
public:
explicit CameraSource(CameraFrame f){
    this->Frame=f;
};
int width()const override{
    return Frame.w;

}
int height()const override{
    return Frame.h;
}
unsigned char at(int x,int y)const override{

return Frame.buf[y*Frame.w + x];
}
    
/* 实现三个虚函数 */ 
};

class RosImage   : public ImageSource {
private:
RosImage Frame;
public: 
 
explicit RosImage(RosImage f){
    this->Frame=f;
}; 
int width()const override{
    return Frame.w;

}
int height()const override{
    return Frame.h;
}
unsigned char at(int x,int y)const override{

return Frame.data[y*Frame.step + x];
  
 /* 同上 */ 
}
};

class VideoFrame : public ImageSource { 
private:
VideoFrame Frame;
public:
explicit VideoFrame(VideoFrame f){
    this->Frame=f;
}; 
int width()const override{
    return Frame.w;

}
int height()const override{
    return Frame.h;
}
unsigned char at(int x,int y)const override{

      return max(Frame.yuv[y*Frame.w + x] - 16, 0);  
   
 /* 同上 */ 
}
};
//双层for循环，题目要求两个版本一致就行
uint64_t preprocess(const ImageSource& src){
    uint64_t sum=0;
    int w=src.width();
    int h=src.height();
    for(int y=0;y<h;y++){
        for(int x=0;x<w;x++){
            sum+=src.at(x,y);
        }
    }
    return sum;
};
}