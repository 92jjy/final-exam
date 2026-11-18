#pragma once

#include "simulator.hpp"

class MyScheduler {
public:
    // 电梯 e 空闲时，读取当前状态并返回下一步动作。
    Action decide(const View& v, int e);
};
