
#ifndef OSM_H
#define OSM_H

#include "../coordinates.hpp"
#include "../graph.hpp"

struct OsmNode {
    u64 id;
    Coordinates coordinates;
    bool mapMatch = true;
};

struct OsmXmlData {
    Graph<OsmNode> graph;
    Coordinates minCoords, maxCoords;
};

OsmXmlData parseOsmXml(const char* path);

#endif
