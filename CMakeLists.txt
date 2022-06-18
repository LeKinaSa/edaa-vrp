cmake_minimum_required(VERSION 3.10)

project(cvrp)

set(GCC_FLAGS_OPTIMIZE "-Ofast -fno-signed-zeros -fno-trapping-math -frename-registers -funroll-loops")

include_directories(lib)
add_subdirectory(lib/GraphViewerCpp)

add_executable(${PROJECT_NAME}
    src/coordinates.cpp
    src/main.cpp
    src/algorithms/ant_colony.cpp
    src/algorithms/a_star.cpp
    src/algorithms/greedy.cpp
    src/algorithms/simulated_annealing.cpp
    src/algorithms/tabu_search.cpp
    src/analysis/complexity.cpp
    src/analysis/real_data.cpp
    src/cvrp/cvrp.cpp
    src/cvrp/stage_1.cpp
    src/cvrp/visualization.cpp
    src/data_structures/quadtree.cpp
    src/data_structures/kd_tree.cpp
    src/osm/osm.cpp

    lib/tinyxml/tinyxml2.cpp
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 ${GCC_FLAGS_OPTIMIZE}")

target_link_libraries(${PROJECT_NAME} PUBLIC graphviewer)
