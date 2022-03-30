#include <utility>
#include <list>
#include <map>
#include <algorithm>
#include "../data_structures/fibonacci_heap.hpp"
#include "../osm/osm.hpp"
#include "../graph.hpp"
#include "../types.hpp"

using namespace std;

pair<list<u64>, double> aStarSearch(Graph<OsmNode> g, u64 start, u64 end) {
    FibonacciHeap<u64> heap;
    map<u64, u64> predecessorMap;
    map<u64, double> currentCostMap;

    double distance = 0;
    double estimate = 0; // TODO
    currentCostMap[start] = distance;
    predecessorMap[start] = start;
    heap.insert(start, distance + estimate);

    u64 min, nextNode;
    double edgeLength;
    list<pair<u64, double>> edges;
    while (!heap.empty()) {
        min = heap.extractMin();
        edges = g.getEdges(min);
        for (pair<u64, bool> edge : edges) {
            nextNode = edge.first;
            edgeLength = edge.second;
            distance = currentCostMap[min] + edgeLength;

            // Check if the destination node has been reached
            if (nextNode == end) {
                predecessorMap[nextNode] = min;

                u64 node = end;
                list<u64> path = {end};
                while (node != start) {
                    node = predecessorMap[node];
                    path.push_front(node);
                }

                return make_pair<list<u64>, double>(path, distance);
            }

            // Update distance and predecessor and add node to the heap (only if the node is new)
            if (!currentCostMap.count(nextNode)) {
                double estimate = 0; // TODO

                currentCostMap[nextNode] = distance;
                predecessorMap[nextNode] = min;
                heap.insert(nextNode, distance + estimate);
            }
        }
    }

    // Failed to find a path between start and end
    return make_pair<list<u64>, double>({}, 0);
}
