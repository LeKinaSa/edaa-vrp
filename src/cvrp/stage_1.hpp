
#ifndef CVRP_STAGE_1_H
#define CVRP_STAGE_1_H

#include <unordered_map>
#include "../types.hpp"
#include "../osm/osm.hpp"
#include "cvrp.hpp"

enum MapMatchingDataStructure {
    QUADTREE,
    KD_TREE,
};

struct MapMatchingResult {
    u64 originNode;
    std::vector<u64> deliveryNodes;
};

// Maps LoggiBUD location IDs to the IDs of nodes in the OSM network
MapMatchingResult matchLocations(const OsmXmlData& osmData,
    const CvrpInstance& problem, MapMatchingDataStructure dataStructure = KD_TREE,
    bool printLogs = false);

void calculateShortestPaths(const OsmXmlData& osmData, CvrpInstance& problem,
    const MapMatchingResult& mmResult, bool printLogs = false);

#endif // CVRP_STAGE_1_H
