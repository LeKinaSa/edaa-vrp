
#include "stage_1.hpp"
#include "../data_structures/quadtree.hpp"
#include "../data_structures/kd_tree.hpp"

using namespace std;

MapMatchingResult matchLocations(const OsmXmlData& osmData,
        const CvrpInstance& problem, MapMatchingDataStructure dataStructure) {
    u64 originNode = 0;
    vector<u64> deliveryNodes;
    deliveryNodes.reserve(problem.getDeliveries().size());

    if (dataStructure == QUADTREE) {
        Quadtree tree(AABB(osmData.minCoords, osmData.maxCoords));
        for (const pair<u64, const OsmNode&>& p : osmData.graph.getNodes()) {
            tree.insert(p.second);
        }

        originNode = tree.nearestNeighbor(problem.getOrigin())->id;
        for (const CvrpDelivery& delivery : problem.getDeliveries()) {
            const OsmNode* node = tree.nearestNeighbor(delivery.coordinates);
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

        originNode = tree.nearestNeighbor(problem.getOrigin())->id;
        for (const CvrpDelivery& delivery : problem.getDeliveries()) {
            const OsmNode* node = tree.nearestNeighbor(delivery.coordinates);
            if (node) {
                deliveryNodes.push_back(node->id);
            }
            else {
                deliveryNodes.push_back(0);
            }
        }
    }

    return {originNode, deliveryNodes};
}