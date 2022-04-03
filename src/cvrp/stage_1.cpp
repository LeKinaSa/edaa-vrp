
#include <chrono>
#include "stage_1.hpp"
#include "../algorithms/a_star.hpp"
#include "../data_structures/quadtree.hpp"
#include "../data_structures/kd_tree.hpp"
#include "../utils.hpp"

using namespace std;
using chrono::high_resolution_clock;
using chrono::microseconds;
using chrono::nanoseconds;

MapMatchingResult matchLocations(const OsmXmlData& osmData,
        const CvrpInstance& problem, MapMatchingDataStructure dataStructure,
        bool printLogs) {
    u64 originNode = 0;
    vector<u64> deliveryNodes;
    const u32 numDeliveries = problem.getDeliveries().size();
    deliveryNodes.reserve(numDeliveries);

    // Log-related variables
    const u32 nnIterations = 1 + numDeliveries;
    u64 timeElapsed = 0;
    high_resolution_clock::time_point start, end;

    if (dataStructure == QUADTREE) {
        Quadtree tree(AABB(osmData.minCoords, osmData.maxCoords));
        for (const pair<u64, const OsmNode&>& p : osmData.graph.getNodes()) {
            tree.insert(p.second);
        }

        start = high_resolution_clock::now();
        originNode = tree.nearestNeighbor(problem.getOrigin())->id;
        end = high_resolution_clock::now();
        timeElapsed += interval<nanoseconds>(start, end);

        for (const CvrpDelivery& delivery : problem.getDeliveries()) {
            start = high_resolution_clock::now();
            const OsmNode* node = tree.nearestNeighbor(delivery.coordinates);
            end = high_resolution_clock::now();
            timeElapsed += interval<nanoseconds>(start, end);

            if (node) {
                deliveryNodes.push_back(node->id);
            }
            else {
                deliveryNodes.push_back(0);
            }
        }
    }
    else {
        vector<reference_wrapper<const OsmNode>> v;
        v.reserve(osmData.graph.getNodes().size());

        for (const pair<u64, const OsmNode&>& p : osmData.graph.getNodes()) {
            v.push_back(p.second);
        }

        KDTree tree(v);

        start = high_resolution_clock::now();
        originNode = tree.nearestNeighbor(problem.getOrigin())->id;
        end = high_resolution_clock::now();
        timeElapsed += interval<nanoseconds>(start, end);

        for (const CvrpDelivery& delivery : problem.getDeliveries()) {
            start = high_resolution_clock::now();
            const OsmNode* node = tree.nearestNeighbor(delivery.coordinates);
            end = high_resolution_clock::now();
            timeElapsed += interval<nanoseconds>(start, end);

            if (node) {
                deliveryNodes.push_back(node->id);
            }
            else {
                deliveryNodes.push_back(0);
            }
        }
    }

    if (printLogs) {
        cout << "Map matched " << nnIterations << " nodes in "
            << timeElapsed / 1000 << "us (average of " << timeElapsed / nnIterations
            << "ns per iteration)\n";
    }

    return {originNode, deliveryNodes};
}

void calculateShortestPaths(const OsmXmlData& osmData, CvrpInstance& problem,
        const MapMatchingResult& mmResult, bool printLogs) {
    size_t n = 1 + problem.getDeliveries().size();

    auto matchedPoint = [mmResult](size_t idx) {
        if (idx == 0) {
            return mmResult.originNode;
        }
        return mmResult.deliveryNodes[idx - 1];
    };

    // We can't assume d[from, to] == d[to, from] since there are directed edges
    for (size_t from = 0; from < n; ++from) {
        for (size_t to = 0; to < n; ++to) {
            if (from != to) {
                pair<list<u64>, double> result = aStarSearch(
                    osmData.graph,
                    matchedPoint(from),
                    matchedPoint(to)
                );

                cout << "(" << from << ", " << to << ") - " << result.second << endl;

                if (result.first.size() == 0) {
                    // Couldn't find a path...
                    problem.setDistance(from, to, DBL_MAX);
                }
                else {
                    problem.setDistance(from, to, result.second);
                }
            }
        }
    }
}