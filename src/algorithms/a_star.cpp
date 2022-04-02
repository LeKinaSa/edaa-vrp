
#include <algorithm>
#include <map>
#include "a_star.hpp"
#include "../data_structures/fibonacci_heap.hpp"
#include "../utils.hpp"

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

double NOT_FOUND = -1;

bool contains(std::list<u64> l, u64 value) {
    // TODO: put in utils
    for (auto v : l) {
        if (v == value) {
            return true;
        }
    }
    return false;
}

pair<bool, pair<list<u64>, double>> search(Graph<OsmNode> graph, list<u64> path, u64 end, double g, double bound) {
    u64 current = path.back();
    Coordinates endCoords     = graph.getNode(end).coordinates;
    Coordinates currentCoords = graph.getNode(current).coordinates;
    double h = currentCoords.haversine(endCoords);
    double f = g + h;

    if (f > bound) {
        return make_pair(false, make_pair(path, f));
    }
    if (current == end) {
        return make_pair(true, make_pair(path, f));
    }

    pair<bool, pair<list<u64>, double>> found;
    u64 node;
    double t, min = NOT_FOUND;
    list<u64> minPath = path;

    for (pair<u64, double> edge : graph.getEdges(current)) {
        node = edge.first;
        if (contains(path, node)) {
            continue;
        }
        
        path.push_back(node);
        
        found = search(graph, path, end, g + edge.second, bound);
        t = found.second.second;

        if (found.first && t != NOT_FOUND) {
            return found;
        }
        if (min == NOT_FOUND || (t != NOT_FOUND && t < min)) {
            min = t;
            minPath = path;
        }

        path.pop_back();
    }
    return make_pair(false, make_pair(minPath, min));
}

pair<list<u64>, double> iterativeDeepeningAStarSearch(Graph<OsmNode> g, u64 start, u64 end) {
    Coordinates endCoords     = g.getNode(end).coordinates;
    Coordinates currentCoords = g.getNode(start).coordinates;

    double t, bound = currentCoords.haversine(endCoords);
    pair<bool, pair<list<u64>, double>> found = make_pair<bool, pair<list<u64>, double>>(false, make_pair<list<u64>, double>({}, 0));
    while (!found.first) {
        found = search(g, {start}, end, 0, bound);
        t = found.second.second;

        if (found.first) {
            if (t != NOT_FOUND) {
                return found.second;
            }
            break;
        }

        bound = t;
    }
    // Failed to find a path between start and end
    return make_pair<list<u64>, double>({}, 0);
}
