
#include <algorithm>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include "a_star.hpp"
#include "../data_structures/binary_heap.hpp"
#include "../data_structures/fibonacci_heap.hpp"
#include "../utils.hpp"

using namespace std;

vector<ShortestPathResult> dijkstra(const Graph<OsmNode>& g, u64 start,
        const vector<u64>& endVec, ShortestPathDataStructure dataStructure) {
    bool bin = dataStructure == BINARY_HEAP;

    vector<ShortestPathResult> resultVec;
    resultVec.reserve(endVec.size());
    if (endVec.empty()) return resultVec;

    BinaryHeap<u64> binHeap(8192);
    unordered_map<u64, u64> binHeapNodes;
    FibonacciHeap<u64> fibHeap;
    unordered_map<u64, FHNode<u64>*> fibHeapNodes;

    unordered_set<u64> endNodes;
    unordered_map<u64, u64> predecessorMap;
    unordered_map<u64, double> distanceMap;

    endNodes.insert(endVec.begin(), endVec.end());

    distanceMap[start] = 0;
    
    if (bin)
        binHeapNodes[start] = binHeap.insert(start, 0);
    else
        fibHeapNodes[start] = fibHeap.insert(start, 0);

    u64 next;
    double distance;

    while (!((bin && binHeap.empty()) || (!bin && fibHeap.empty())) && !endNodes.empty()) {
        next = bin ? binHeap.extractMin() : fibHeap.extractMin();
        endNodes.erase(next);

        for (const auto& edge : g.getEdges(next)) {
            distance = distanceMap[next] + edge.second;
            bool seen = distanceMap.count(edge.first) != 0;

            if (!seen || distance < distanceMap[edge.first]) {
                distanceMap[edge.first] = distance;
                predecessorMap[edge.first] = next;

                if (seen) {
                    bin ?
                        binHeap.decreaseKey(binHeapNodes[edge.first], distance) :
                        fibHeap.decreaseKey(fibHeapNodes[edge.first], distance);
                }
                else {
                    if (bin)
                        binHeapNodes[edge.first] = binHeap.insert(edge.first, distance);
                    else
                        fibHeapNodes[edge.first] = fibHeap.insert(edge.first, distance);
                }
            }
        }
    }

    for (const auto& end : endVec) {
        ShortestPathResult result;

        if (end == start) {
            result.path.push_front(end);
            result.path.push_front(start);
        }
        else if (predecessorMap.count(end)) {
            result.distance = distanceMap[end];

            u64 node = end;
            result.path.push_front(node);
            while (predecessorMap.count(node)) {
                node = predecessorMap[node];
                result.path.push_front(node);
            }
        }

        resultVec.push_back(result);
    }

    return resultVec;
}

pair<list<u64>, double> aStarSearch(const Graph<OsmNode>& g, u64 start, u64 end) {
    FibonacciHeap<u64> heap;
    map<u64, FHNode<u64>*> fibHeapNodes;
    map<u64, u64> predecessorMap;
    map<u64, double> gScore;

    Coordinates endCoords     = g.getNode(end).coordinates;
    Coordinates currentCoords = g.getNode(start).coordinates;

    double distance = 0;
    double fScore = distance + currentCoords.haversine(endCoords);
    gScore[start] = distance;
    fibHeapNodes[start] = heap.insert(start, fScore);

    u64 min, neighbor;
    double edgeLength;
    while (!heap.empty()) {
        min = heap.extractMin();
        fibHeapNodes.erase(min);

        // Check if the destination node has been reached
        if (min == end) {
            u64 node = min;
            list<u64> path;
            path.push_front(node);
            while (predecessorMap.count(node)) {
                node = predecessorMap[node];
                path.push_front(node);
            }

            return make_pair(path, distance);
        }

        for (const pair<u64, double>& edge : g.getEdges(min)) {
            neighbor   = edge.first;
            edgeLength = edge.second;
            distance   = gScore[min] + edgeLength;

            bool seen = gScore.count(neighbor) != 0;
            if (!seen || distance < gScore[neighbor]) {
                currentCoords = g.getNode(neighbor).coordinates;
                fScore = distance + currentCoords.haversine(endCoords);

                predecessorMap[neighbor] = min;
                gScore[neighbor] = distance;
                if (seen) {
                    heap.decreaseKey(fibHeapNodes[neighbor], fScore);
                }
                else {
                    fibHeapNodes[neighbor] = heap.insert(neighbor, fScore);
                }
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
