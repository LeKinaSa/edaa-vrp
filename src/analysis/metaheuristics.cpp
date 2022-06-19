
#include <chrono>
#include <fstream>
#include <iomanip>
#include <functional>

#include "../algorithms/ant_colony.hpp"
#include "../algorithms/greedy.hpp"
#include "../algorithms/simulated_annealing.hpp"
#include "../algorithms/tabu_search.hpp"
#include "metaheuristics.hpp"

using namespace std;

using chrono::high_resolution_clock;
using chrono::microseconds;

static const size_t NUM_INSTANCES = 7;
static const array<string, NUM_INSTANCES> cvrpInstances = {
    "0-pa-61", // ~200
    "0-pa-25", // ~275
    "0-pa-34", // ~350
    "0-df-12", // ~800
    "0-df-15", // ~1000
    "0-df-44", // ~1250
    "2-rj-17", // ~1900
};

CvrpInstance loadInstance(const string& name) {
    ifstream ifs("../cvrp-" + name + ".json");
    CvrpInstance instance(ifs);
    ifs.close();
    instance.readDistanceMatrixFromFile(("dm-" + name + ".txt").c_str());
    return instance;
}

static const char* csvHeader = "name,num_deliveries,solution_length,num_vehicles,average_load,time_us\n";

void printResults(ofstream& ofs, const string& name, const CvrpInstance& instance, const CvrpSolution& solution, u64 us) {
    u32 numVehicles = solution.routes.size();
    double totalCargo = 0;
    for (const auto& delivery : instance.getDeliveries()) {
        totalCargo += delivery.size;
    }
    double averageCargo = totalCargo / numVehicles;

    ofs << setprecision(2) << fixed << name << "," << instance.getDeliveries().size() << ","
        << solution.length << "," << numVehicles << "," << averageCargo << "," << us << "\n";
    cout << name << " finished in " << us / 1000.0 << " ms, solution has length "
        << solution.length / 1000 << " km, used " << numVehicles << " vehicles (average load "
        << averageCargo << ")" << endl;
}

CvrpSolution simulatedAnnealingDefault(const CvrpInstance& instance) {
    return simulatedAnnealing(instance);
}

CvrpSolution granularTabuSearchDefault(const CvrpInstance& instance) {
    return granularTabuSearch(instance, 200);
}

CvrpSolution antColonyOptimizationDefault(const CvrpInstance& instance) {
    AntColonyConfig config;
    config.maxIterations = 100;
    config.eliteAnts = 10;
    return antColonyOptimization(instance, config);
}

void metaheuristicComparison() {
    static const u32 NUM_METAHEURISTICS = 4;

    static const array<const char*, NUM_METAHEURISTICS> fileNames = {
        "greedy_analysis.csv",
        "sa_analysis.csv",
        "gts_analysis.csv",
        "aco_analysis.csv"
    };

    static const array<u32, NUM_METAHEURISTICS> iterations = {1, 5, 1, 1};
    static array<function<CvrpSolution(const CvrpInstance&)>, NUM_METAHEURISTICS> functions = {
        greedyAlgorithm,
        simulatedAnnealingDefault,
        granularTabuSearchDefault,
        antColonyOptimizationDefault,
    };

    array<ofstream, NUM_METAHEURISTICS> fileStreams;
    for (size_t i = 0; i < NUM_METAHEURISTICS; ++i) {
        fileStreams[i] = ofstream(fileNames[i]);
    }

    for (const auto& name : cvrpInstances) {
        CvrpInstance instance = loadInstance(name);

        for (size_t i = 0; i < NUM_METAHEURISTICS; ++i) {
            for (size_t iter = 0; i < iterations[i]; ++iter) {
                auto start = high_resolution_clock::now();
                CvrpSolution solution = functions[i](instance);
                auto end = high_resolution_clock::now();
                
                printResults(fileStreams[i], name, instance, solution, interval<chrono::microseconds>(start, end));
            }
        }
    }
}

void simulatedAnnealingAnalysis() {

}

void granularTabuSearchAnalysis() {

}

void antColonyOptimizationAnalysis() {

}
