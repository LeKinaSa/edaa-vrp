
#include <algorithm>
#include <map>
#include "a_star.hpp"
#include "../data_structures/fibonacci_heap.hpp"

using namespace std;

pair<list<u64>, double> aStarSearch(Graph<OsmNode> g, u64 start, u64 end) {
    FibonacciHeap<u64> heap;
    map<u64, u64> predecessorMap;
    map<u64, double> currentCostMap;

    Coordinates endCoords     = g.getNode(end).coordinates;
    Coordinates currentCoords = g.getNode(start).coordinates;

    double distance = 0;
    double estimate = currentCoords.haversine(endCoords);
    currentCostMap[start] = distance;
    predecessorMap[start] = start;
    heap.insert(start, distance + estimate);

    u64 min, nextNode;
    double edgeLength;
    list<pair<u64, double>> edges;
    while (!heap.empty()) {
        min = heap.extractMin();
        edges = g.getEdges(min);
        for (pair<u64, double> edge : edges) {
            nextNode   = edge.first;
            edgeLength = edge.second;
            distance   = currentCostMap[min] + edgeLength;

            // Check if the destination node has been reached
            if (nextNode == end) {
                predecessorMap[nextNode] = min;

                u64 node = end;
                list<u64> path;
                path.push_front(node);
                while (node != start) {
                    node = predecessorMap[node];
                    path.push_front(node);
                }
                pair<list<u64>, double> p = make_pair(path, distance);
                return p; // TODO: check if this breaks (make_pair was not working)
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
    return make_pair<list<u64>, double>({}, 0);
}