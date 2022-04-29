
#ifndef VISUALIZATION_H
#define VISUALIZATION_H

#include <unordered_map>
#include <GraphViewerCpp/include/graphviewer.h>
#include "../osm/osm.hpp"
#include "../graph.hpp"
#include "cvrp.hpp"
#include "stage_1.hpp"

struct GraphVisualizationResult {
    GraphViewer* gv;
    std::unordered_map<u64, std::unordered_map<u64, u64>> edgeIds;

    GraphVisualizationResult() {
        gv = new GraphViewer();
    }

    ~GraphVisualizationResult() {
        delete gv;
    }

    GraphViewer& gvRef() {
        return *gv;
    }
};

GraphVisualizationResult generateGraphVisualization(const OsmXmlData& data, float scale = 200000.0);
void setGraphCenter(GraphViewer& gv, const Coordinates& coords, float scale = 200000.0);
void showMapMatchingResults(GraphViewer& gv, const CvrpInstance& instance,
    const MapMatchingResult& result, float scale = 200000.0);
void highlightPath(GraphVisualizationResult& result, const std::list<u64>& path, const sf::Color& color = sf::Color::Red);

#endif // VISUALIZATION_H
