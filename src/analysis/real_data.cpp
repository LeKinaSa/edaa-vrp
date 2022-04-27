
#include <chrono>
#include <fstream>
#include "real_data.hpp"
#include "../algorithms/a_star.hpp"
#include "../utils.hpp"

using namespace std;

using chrono::high_resolution_clock;
using chrono::milliseconds;

void shortestPathDataStructureAnalysis() {
    auto printPaths = [](const OsmXmlData& data, CvrpInstance& instance,
            const MapMatchingResult& result, ShortestPathDataStructure dataStructure) {
        const char* dsName = dataStructure == BINARY_HEAP ? "Binary Heap" : "Fibonacci Heap";
        
        auto start = high_resolution_clock::now();
        calculateShortestPaths(data, instance, result, dataStructure, false, 12);
        auto end = high_resolution_clock::now();

        cout << dsName << "\n" << interval<chrono::milliseconds>(start, end) << endl;
    };

    {
        OsmXmlData data = parseOsmXml("../cvrp_belem.xml");
        ifstream ifs("../cvrp-0-pa-34.json");
        CvrpInstance instance(ifs);
        MapMatchingResult result = matchLocations(data, instance, KD_TREE, true);

        printPaths(data, instance, result, BINARY_HEAP);
        printPaths(data, instance, result, FIBONACCI_HEAP);
    }

    {
        OsmXmlData data = parseOsmXml("../cvrp_brasilia.xml");
        ifstream ifs("../cvrp-0-df-12.json");
        CvrpInstance instance(ifs);
        MapMatchingResult result = matchLocations(data, instance, KD_TREE, true);

        printPaths(data, instance, result, BINARY_HEAP);
        printPaths(data, instance, result, FIBONACCI_HEAP);
    }

    {
        OsmXmlData data = parseOsmXml("../cvrp_rio.xml");
        ifstream ifs("../cvrp-2-rj-17.json");
        CvrpInstance instance(ifs);
        MapMatchingResult result = matchLocations(data, instance, KD_TREE, true);

        printPaths(data, instance, result, BINARY_HEAP);
        printPaths(data, instance, result, FIBONACCI_HEAP);
    }
}

void parallelismAnalysis() {
    auto printPaths = [](const OsmXmlData& data, CvrpInstance& instance,
            const MapMatchingResult& result, ShortestPathDataStructure dataStructure,
            size_t numThreads, const string& filePath) {
        auto start = high_resolution_clock::now();
        calculateShortestPaths(data, instance, result, dataStructure, true, numThreads, filePath);
        auto end = high_resolution_clock::now();

        cout << interval<chrono::milliseconds>(start, end) << endl;
    };

    OsmXmlData data = parseOsmXml("../cvrp_belem.xml");
    ifstream ifs("../cvrp-0-pa-34.json");
    CvrpInstance instance(ifs);
    MapMatchingResult result = matchLocations(data, instance, KD_TREE, true);

    cout << "FIBONACCI HEAP\n--------------\n";
    for (size_t i = 1; i <= 24; ++i) {
        cout << "[ " << i << " THREADS ] - ";
        printPaths(data, instance, result, FIBONACCI_HEAP, i, "shortest_paths_f" + to_string(i) + ".txt");
    }

    cout << "BINARY HEAP\n-----------\n";
    for (size_t i = 1; i <= 24; ++i) {
        cout << "[ " << i << " THREADS ] - ";
        printPaths(data, instance, result, BINARY_HEAP, i, "shortest_paths_b" + to_string(i) + ".txt");
    }
}
