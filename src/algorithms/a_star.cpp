
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

                return make_pair(path, distance);
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

std::pair<std::list<u64>, double> simpleMemoryBoundedAStarSearch(Graph<OsmNode> g, u64 start, u64 end, int maxSize) {
    FibonacciHeap<u64> heap;
    map<u64, u64> predecessorMap;
    map<u64, double> currentCostMap;

    Coordinates endCoords     = g.getNode(end).coordinates;
    Coordinates currentCoords = g.getNode(start).coordinates;

    double distance = 0;
    double estimate = currentCoords.haversine(endCoords);
    double totalDistance = distance + estimate;

    currentCostMap[start] = distance;
    predecessorMap[start] = start;
    heap.insert(start, totalDistance);

    u64 min, nextNode, max;
    double edgeLength, maxF;
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

                return make_pair(path, distance);
            }

            // Update distance and predecessor and add node to the heap (only if the node is new)
            if (!currentCostMap.count(nextNode)) {
                currentCoords = g.getNode(nextNode).coordinates;
                estimate = currentCoords.haversine(endCoords);
                totalDistance = distance + estimate;

                if (heap.getSize() == maxSize) {
                    max = heap.extractMax();

                    currentCoords = g.getNode(max).coordinates;
                    estimate = currentCoords.haversine(endCoords);
                    maxF = currentCostMap[max] + estimate;

                    if (totalDistance > maxF) {
                        // The nextNode has a bigger F than the current Max (max gets re-added and nextNode is ignored)
                        heap.insert(max, maxF);
                        continue;
                    }
                    else {
                        // Remove the max from the maps
                        currentCostMap.erase(max);
                        predecessorMap.erase(max);
                    }
                }
                
                currentCostMap[nextNode] = distance;
                predecessorMap[nextNode] = min;
                heap.insert(nextNode, totalDistance);
            }
        }
    }

    // Failed to find a path between start and end
    return make_pair<list<u64>, double>({}, 0);
}
