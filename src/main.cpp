#include <iostream>
#include <fstream>

#include <GraphViewerCpp/include/graphviewer.h>
#include <cxxopts/cxxopts.hpp>

#include "algorithms/a_star.hpp"
#include "analysis/complexity.hpp"
#include "analysis/real_data.hpp"
#include "cvrp/cvrp.hpp"
#include "cvrp/stage_1.hpp"
#include "cvrp/visualization.hpp"
#include "utils.hpp"

using namespace std;

int main(int argc, char** argv) {
    cxxopts::Options opts("cvrp", "Solver for large CVRP instances from the LoggiBUD dataset");

    opts.set_width(100)
        .allow_unrecognised_options()
        .add_options()
        ("cvrp", "[REQ] Path to CVRP JSON file", cxxopts::value<string>())
        ("osm", "[REQ] Path to OSM XML file", cxxopts::value<string>())
        ("dm", "[OPT] Path to distance matrix", cxxopts::value<string>())
        ("vmm", "[OPT] Visualize map matching")
        ("vsp", "[OPT] Visualize shortest paths (for depot point)")
        ("t,threads", "[OPT] Number of threads to use in shortest path calculation", cxxopts::value<u32>()->default_value("1"))
        ("h,help", "[OPT] Print usage")
        ("l,logs", "[OPT] Enable additional execution logs")
        ("quadtree", "[OPT] Use quadtrees instead of k-d trees for map matching")
        ("bin-heap", "[OPT] Use binary heaps instead of Fibonacci heaps for Dijkstra's algorithm")
        ;

    auto result = opts.parse(argc, argv);

    if (result.arguments().empty() || result.count("help")) {
        cout << opts.help() << endl;
        exit(0);
    }

    bool logs = result["logs"].as<bool>();
    u32 threads = result["threads"].as<u32>();

    bool mmVis = result["vmm"].as<bool>(), spVis = result["vsp"].as<bool>();
    
    MapMatchingDataStructure mmDataStructure = result["quadtree"].as<bool>() ? QUADTREE : KD_TREE;
    ShortestPathDataStructure spDataStructure = result["bin-heap"].as<bool>() ? BINARY_HEAP : FIBONACCI_HEAP;

    if (result.count("cvrp") && result.count("osm")) {
        string cvrpPath = result["cvrp"].as<string>(),
            osmPath = result["osm"].as<string>();

        {
            ifstream i1(cvrpPath), i2(osmPath);

            if (!i1.good() || !i2.good()) {
                cerr << "Error: `cvrp` and `osm` must be paths to regular files." << endl;
                exit(1);
            }
        }

        cout << "Parsing OSM XML..." << endl;
        OsmXmlData data = parseOsmXml(osmPath.c_str());

        cout << "Parsing CVRP instance..." << endl;
        ifstream ifs(cvrpPath);
        CvrpInstance instance(ifs);

        cout << "Generating graph visualization..." << endl;
        GraphVisualizationResult gvr = generateGraphVisualization(data);
        GraphViewer& gv = gvr.gvRef();

        cout << "Matching coordinates to OSM network nodes..." << endl;
        MapMatchingResult mmResult = matchLocations(data, instance, mmDataStructure, logs);
        showMapMatchingResults(gv, instance, mmResult);
        setGraphCenter(gv, instance.getOrigin());
        gv.setZipEdges(true);
        gv.setEnabledEdgesText(false);

        if (mmVis) {
            gv.createWindow(1280, 720);
            gv.join();
            gv.closeWindow();
        }

        string dmPath = "";
        if (result.count("dm")) {
            dmPath = result["dm"].as<string>();
        }
        bool readFromFile = false;

        {
            ifstream ifsDm(dmPath);

            if (ifsDm.is_open()) {
                instance.readDistanceMatrixFromFile(dmPath.c_str());
                readFromFile = true;
            }
        }

        if (!readFromFile) {
            cout << "Calculating shortest paths between matched nodes..." << endl;
            calculateShortestPaths(data, instance, mmResult, spDataStructure, logs, threads);
            if (spVis) {
                vector<ShortestPathResult> spResult = dijkstra(data.graph,
                    mmResult.originNode, mmResult.deliveryNodes, spDataStructure);

                for (const auto& res : spResult) {
                    highlightPath(gvr, res.path);
                }
                gv.setZipEdges(true);

                gv.createWindow(1280, 720);
                gv.join();
                gv.closeWindow();
            }

            if (!dmPath.empty()) {
                instance.writeDistanceMatrixToFile(dmPath.c_str());
            }
        }
    }
    else {
        cerr << "Error: `cvrp` and `osm` options are required." << endl;
        exit(1);
    }

    return 0;
}
