
#include "visualization.hpp"
#include "../algorithms/a_star.hpp"
#include <unordered_map>
#include <algorithm>
#include <random>
#include <chrono>

using namespace std;

sf::Vector2f vecFromCoordinates(const Coordinates& coords, float scale = 1.0) {
    return sf::Vector2f(coords.getLongitude() * scale, coords.getLatitude() * scale);
}

GraphVisualizationResult* generateGraphVisualization(const OsmXmlData& data, float scale) {
    GraphVisualizationResult* resultPtr = new GraphVisualizationResult();
    GraphVisualizationResult& result = *resultPtr;

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

    return resultPtr;
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

static const u32 MAX_RGB = 510;

void showSolution(GraphVisualizationResult& result, const MapMatchingResult& mmResult, const Graph<OsmNode>& graph, const CvrpSolution& solution) {
    auto matchedNode = [&mmResult](u64 idx) {
        return idx == 0 ? mmResult.originNode : mmResult.deliveryNodes[idx - 1];
    };

    auto randomEngine = default_random_engine(
        chrono::system_clock::now().time_since_epoch().count()
    );

    auto& origin = result.gv->getNode(mmResult.originNode);
    origin.setColor(sf::Color::Green);
    origin.setSize(60);
    origin.setLabel("O");
    origin.setLabelSize(30);
    origin.enable();

    for (const auto& route : solution.routes) {
        array<u32, 3> rgb = {0, 0, 0};
        rgb[0] = min(MAX_RGB, (u32) rand() % 256);
        rgb[1] = min(MAX_RGB - rgb[0], (u32) rand() % 256);
        rgb[2] = min(MAX_RGB - rgb[0] - rgb[1], (u32) rand() % 256);
        shuffle(rgb.begin(), rgb.end(), randomEngine);

        auto color = sf::Color(rgb[0], rgb[1], rgb[2]);

        for (int i = 0; i < route.size() - 1; ++i) {
            u64 from = matchedNode(route[i]), to = matchedNode(route[i + 1]);
            list<u64> path = aStarSearch(graph, from, to).first;
            highlightPath(result, path, color);

            if (route[i + 1] != 0) {
                auto& node = result.gv->getNode(to);
                node.setColor(color);
                node.setSize(45);
                node.setLabel(to_string(route[i + 1]));
                node.setLabelSize(20);
                node.enable();
            }
        }
    }
}
