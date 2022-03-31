
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

double NOT_FOUND = -1;

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
    double t, min = NOT_FOUND;

    for (pair<u64, double> edge : graph.getEdges(current)) {
        path.push_back(edge.first);

        found = search(graph, path, end, g + edge.second, bound);
        t = found.second.second;

        if (found.first && t != NOT_FOUND) {
            return found;
        }
        if (min == NOT_FOUND || (t != NOT_FOUND && t < min)) {
            min = t;
        }

        path.pop_back();
    }
    return make_pair(true, make_pair(path, NOT_FOUND));
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
    return make_pair<list<u64>, double>({}, 0);
}
