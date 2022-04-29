#include <iostream>
#include <filesystem>
#include <fstream>

#include <GraphViewerCpp/include/graphviewer.h>
#include <cxxopts/cxxopts.hpp>

#include "analysis/complexity.hpp"
#include "analysis/real_data.hpp"
#include "cvrp/cvrp.hpp"
#include "cvrp/stage_1.hpp"
#include "cvrp/visualization.hpp"
#include "utils.hpp"

using namespace std;

int main(int argc, char** argv) {
    cxxopts::Options opts("LoggiCVRP", "Solver for large CVRP instances from the LoggiBUD dataset");

    opts.allow_unrecognised_options()
        .add_options()
        ("cvrp", "[REQ] Path to CVRP JSON file", cxxopts::value<string>())
        ("osm", "[REQ] Path to OSM XML file", cxxopts::value<string>())
        ("vmm", "[OPT] Visualize map matching")
        ("vsp", "[OPT] Visualize shortest paths (for depot point)")
        ("t,threads", "[OPT] Number of threads to use in shortest path calculation", cxxopts::value<u32>()->default_value("1"))
        ("h,help", "[OPT] Print usage")
        ("l,logs", "[OPT] Enable additional execution logs")
        ;

    auto result = opts.parse(argc, argv);

    if (result.arguments().empty() || result.count("help")) {
        cout << opts.help() << endl;
        exit(0);
    }

    bool logs = result["logs"].as<bool>();
    u32 threads = result["threads"].as<u32>();

    bool mmVis = result["vmm"].as<bool>(), spVis = result["vsp"].as<bool>();

    if (result.count("cvrp") && result.count("osm")) {
        string cvrpPath = result["cvrp"].as<string>(),
            osmPath = result["osm"].as<string>();

        if (!filesystem::is_regular_file(cvrpPath) || !filesystem::is_regular_file(osmPath)) {
            cerr << "Error: `cvrp` and `osm` must be paths to regular files." << endl;
            exit(1);
        }

        OsmXmlData data = parseOsmXml(osmPath.c_str());
        ifstream ifs(cvrpPath);
        CvrpInstance instance(ifs);

        GraphVisualizationResult gvr = generateGraphVisualization(data);
        GraphViewer& gv = gvr.gvRef();

        MapMatchingResult mmResult = matchLocations(data, instance, KD_TREE, logs);
        if (mmVis) {
            showMapMatchingResults(gv, instance, mmResult);
            setGraphCenter(gv, instance.getOrigin());
            gv.createWindow(1280, 720);
            gv.join();
            gv.closeWindow();
        }

        calculateShortestPaths(data, instance, mmResult, FIBONACCI_HEAP, logs, threads);
        if (spVis) {
            
        }
    }
    else {
        cerr << "Error: `cvrp` and `osm` options are required." << endl;
        exit(1);
    }

    return 0;
}
