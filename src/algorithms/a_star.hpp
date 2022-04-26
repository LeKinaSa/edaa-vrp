
#ifndef A_STAR_H
#define A_STAR_H

#include <utility>
#include <list>
#include <vector>
#include "../osm/osm.hpp"
#include "../graph.hpp"
#include "../types.hpp"
#include "../cvrp/stage_1.hpp"

struct ShortestPathResult {
    std::list<u64> path;
    double distance;
};

std::vector<ShortestPathResult> dijkstra(const Graph<OsmNode>& g, u64 start,
    const std::vector<u64>& endVec, ShortestPathDataStructure dataStructure);

std::pair<std::list<u64>, double> aStarSearch(const Graph<OsmNode>& g, u64 start, u64 end);

std::pair<std::list<u64>, double> simpleMemoryBoundedAStarSearch(Graph<OsmNode> g, u64 start, u64 end, int maxSize);

std::pair<std::list<u64>, double> iterativeDeepeningAStarSearch(Graph<OsmNode> g, u64 start, u64 end);

#endif // A_STAR_H
