
cmake_minimum_required(VERSION 3.16)

project(TASK7)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(app
    main.cpp
    pipeline.cpp
    simulation.cpp
    solver.cpp
)

find_package(Threads REQUIRED)

target_link_libraries(app PRIVATE Threads::Threads)
