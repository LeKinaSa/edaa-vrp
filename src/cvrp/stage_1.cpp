
#include "stage_1.hpp"
#include "../data_structures/quadtree.hpp"
#include "../data_structures/kd_tree.hpp"

using namespace std;

MapMatchingResult matchLocations(const OsmXmlData& osmData,
        const CvrpInstance& problem, MapMatchingDataStructure dataStructure) {
    u64 originNode = 0;
    vector<u64> deliveryNodes;

    if (dataStructure == QUADTREE) {
        Quadtree q(AABB(osmData.minCoords, osmData.maxCoords));
        for (const pair<u64, const OsmNode&>& p : osmData.graph.getNodes()) {
            q.insert(p.second);
        }

        originNode = q.nearestNeighbor(problem.getOrigin())->id;
        for (const CvrpDelivery& delivery : problem.getDeliveries()) {
            deliveryNodes.push_back(q.nearestNeighbor(delivery.coordinates)->id);
        }
    }
    else {

    }

    return {originNode, deliveryNodes};
}