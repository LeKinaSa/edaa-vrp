
#include "visualization.hpp"
#include <unordered_map>

using namespace std;

sf::Vector2f vecFromCoordinates(const Coordinates& coords, float scale = 1.0) {
    return sf::Vector2f(coords.getLongitude() * scale, coords.getLatitude() * scale);
}

GraphVisualizationResult generateGraphVisualization(const OsmXmlData& data, float scale) {
    GraphVisualizationResult result;

    // TODO: make this a parameter?
    result.gv->setScale(5);

    for (const auto& p : data.graph.getNodes()) {
        auto& node = result.gv->addNode(p.first, vecFromCoordinates(p.second.coordinates, scale));
        node.disable();
        result.edgeIds[p.first] = {};
    }

    u64 eId = 0;
    for (const auto& nodeP : data.graph.getNodes()) {
        for (const auto& edgeP : data.graph.getEdges(nodeP.first)) {
            if (result.edgeIds.count(edgeP.first)) {
                if (result.edgeIds.at(edgeP.first).count(nodeP.first)) {
                    // Edge is bidirectional, this prevents drawing it twice
                    result.edgeIds[nodeP.first][edgeP.first] = result.edgeIds[edgeP.first][nodeP.first];
                    continue;
                }
            }

            auto& edge = result.gv->addEdge(eId, result.gv->getNode(nodeP.first),
                result.gv->getNode(edgeP.first));
            edge.setThickness(8);
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

void showMapMatchingResults(GraphViewer& gv, const CvrpInstance& instance,
        const MapMatchingResult& result, float scale) {
    // Use negative IDs to avoid collisions
    for (size_t i = 0; i < instance.getDeliveries().size() + 1; ++i) {
        Coordinates coords = instance.getOrigin();
        u64 matchedId = result.originNode;
        string label = "O";
        sf::Color color = sf::Color::Green;

        if (i != 0) {
            coords = instance.getDeliveries().at(i - 1).coordinates;
            matchedId = result.deliveryNodes.at(i - 1);
            label = to_string(i - 1);
            color = sf::Color::Red;
        }

        auto& location = gv.addNode(-i - 1, vecFromCoordinates(coords, scale));
        location.setColor(sf::Color::Blue);
        location.setSize(35);
        location.setLabel(label);
        location.setLabelColor(sf::Color::White);

        auto& matched = gv.getNode(matchedId);
        matched.setColor(color);
        matched.setSize(35);
        matched.setLabel(label);
        matched.enable();

        auto& edge = gv.addEdge(-i - 1, location, matched);
        edge.setDashed(true);
        edge.setColor(sf::Color::Blue);
    }
}

void highlightPath(GraphVisualizationResult& result, const list<u64>& path, const sf::Color& color) {
    GraphViewer& gv = result.gvRef();
    const auto& edgeIds = result.edgeIds;

    if (path.size() > 1) {
        auto it = path.begin();
        while (next(it) != path.end()) {
            u64 edgeId = edgeIds.at(*it).at(*next(it));
            auto& edge = gv.getEdge(edgeId);
            edge.setThickness(15);
            edge.setColor(color);

            ++it;
        }
    }
}
