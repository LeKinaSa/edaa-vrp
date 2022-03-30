
#ifndef A_STAR_H
#define A_STAR_H

#include <utility>
#include <list>
#include <map>
#include <algorithm>
#include "../data_structures/fibonacci_heap.hpp"
#include "../osm/osm.hpp"
#include "../graph.hpp"
#include "../types.hpp"

std::pair<std::list<u64>, double> aStarSearch(Graph<OsmNode> g, u64 start, u64 end) {
    FibonacciHeap<u64> heap;
    std::map<u64, u64> predecessorMap;
    std::map<u64, double> currentCostMap;

    Coordinates endCoords     = g.getNode(end).coordinates;
    Coordinates currentCoords = g.getNode(start).coordinates;

    double distance = 0;
    double estimate = currentCoords.haversine(endCoords);
    currentCostMap[start] = distance;
    predecessorMap[start] = start;
    heap.insert(start, distance + estimate);

    u64 min, nextNode;
    double edgeLength;
    std::list<std::pair<u64, double>> edges;
    while (!heap.empty()) {
        min   = heap.extractMin();
        edges = g.getEdges(min);
        for (std::pair<u64, bool> edge : edges) {
            nextNode   = edge.first;
            edgeLength = edge.second;
            distance   = currentCostMap[min] + edgeLength;

            // Check if the destination node has been reached
            if (nextNode == end) {
                predecessorMap[nextNode] = min;

                u64 node = end;
                std::list<u64> path;
                path.push_front(node);
                while (node != start) {
                    node = predecessorMap[node];
                    path.push_front(node);
                }
                std::pair<std::list<u64>, double>* p = new std::pair<std::list<u64>, double> (path, distance);
                return *p; // TODO: check if this breaks (make_pair was not working)
            }

            // Update distance and predecessor and add node to the heap (only if the node is new)
            if (!currentCostMap.count(nextNode)) {
                currentCoords = g.getNode(nextNode).coordinates;
                double estimate = currentCoords.haversine(endCoords);

                currentCostMap[nextNode] = distance;
                predecessorMap[nextNode] = min;
                heap.insert(nextNode, distance + estimate);
            }
        }
    }

    // Failed to find a path between start and end
    return std::make_pair<std::list<u64>, double>({}, 0);
}

#endif // A_STAR_H
