cmake_minimum_required(VERSION 3.10)

project(cvrp)

include_directories(lib)

add_executable(${PROJECT_NAME}.out
    src/coordinates.cpp
    src/cvrp.cpp
    src/main.cpp
    src/data_structures/quadtree.cpp
    src/osm/osm.cpp

    lib/tinyxml/tinyxml2.cpp
)