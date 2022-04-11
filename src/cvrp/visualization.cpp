
#include "visualization.hpp"
#include <unordered_map>

using namespace std;

sf::Vector2f vecFromCoordinates(const Coordinates& coords, float scale = 1.0) {
    return sf::Vector2f(coords.getLongitude() * scale, coords.getLatitude() * scale);
}

GraphVisualizationResult generateGraphVisualization(const OsmXmlData& data, float scale) {
    GraphVisualizationResult result;

    // TODO: make this a parameter?
    result.gv->setScale(20);

    for (const auto& p : data.graph.getNodes()) {
        auto& node = result.gv->addNode(p.first, vecFromCoordinates(p.second.coordinates, scale));
        node.disable();
        result.edgeIds[p.first] = {};
    }

    u64 eId = 0;
    for (const auto& nodeP : data.graph.getNodes()) {
        for (const auto& edgeP : data.graph.getEdges(nodeP.first)) {
            auto& edge = result.gv->addEdge(eId, result.gv->getNode(nodeP.first),
                result.gv->getNode(edgeP.first));
            result.edgeIds[nodeP.first][edgeP.first] = eId;
            ++eId;
        }
    }

    Coordinates center(
        (data.maxCoords.getLatitude() + data.minCoords.getLatitude()) / 2,
        (data.maxCoords.getLongitude() + data.minCoords.getLongitude()) / 2
    );
    setGraphCenter(*(result.gv), center, scale);

    return result;
}

void setGraphCenter(GraphViewer& gv, const Coordinates& coords, float scale) {
    gv.setCenter(vecFromCoordinates(coords, scale));
}
