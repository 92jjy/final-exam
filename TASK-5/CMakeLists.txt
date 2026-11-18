cmake_minimum_required(VERSION 3.16)

# 项目名称
project(Elevator)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 生成可执行程序,命名app
add_executable(app
    main.cpp
    simulator.cpp
    my_scheduler.cpp
)
