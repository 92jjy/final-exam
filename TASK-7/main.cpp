#include "serial.hpp"
#include"pipeline.hpp"
int main() {
    //启用多线程版本
    run_pipeline(2);  // 同时推理的帧数上限
    
}