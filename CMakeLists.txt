cmake_minimum_required(VERSION 3.10)

project(cvrp)

include_directories(include)

add_executable(${PROJECT_NAME}.out
    src/main.cpp
    src/coordinates.cpp
)