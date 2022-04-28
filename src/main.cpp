#include <iostream>

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
        ;

    auto result = opts.parse(argc, argv);

    if (result.arguments().empty() || result.count("help")) {
        cout << opts.help() << endl;
        exit(0);
    }

    if (result.count("cvrp") && result.count("osm")) {
        // TODO
    }
    else {
        cerr << "Error: `cvrp` and `osm` options are required." << endl;
    }

    return 0;
}
