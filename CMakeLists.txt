cmake_minimum_required(VERSION 3.10)

project(cvrp)

include_directories(lib)
add_subdirectory(lib/GraphViewerCpp)

add_executable(${PROJECT_NAME}
    src/coordinates.cpp
    src/cvrp.cpp
    src/main.cpp
    src/algorithms/a_star.cpp
    src/analysis/complexity.cpp
    src/data_structures/quadtree.cpp
    src/data_structures/kd_tree.cpp
    src/osm/osm.cpp

    lib/tinyxml/tinyxml2.cpp
)

target_link_libraries(${PROJECT_NAME} PUBLIC graphviewer)
