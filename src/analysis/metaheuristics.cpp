
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

CvrpSolution greedyAlgorithmDefault(const CvrpInstance& instance) {
    return greedyAlgorithm(instance);
}

CvrpSolution clarkeWrightSavingsDefault(const CvrpInstance& instance) {
    return clarkeWrightSavings(instance);
}

CvrpSolution simulatedAnnealingDefault(const CvrpInstance& instance) {
    SimulatedAnnealingConfig config;
    return simulatedAnnealing(instance, config);
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
    static const u32 NUM_METAHEURISTICS = 5;

    static const array<const char*, NUM_METAHEURISTICS> fileNames = {
        "greedy_analysis.csv",
        "clarke_wright_analysis.csv",
        "sa_analysis.csv",
        "gts_analysis.csv",
        "aco_analysis.csv"
    };

    static const array<u32, NUM_METAHEURISTICS> iterations = {1, 1, 5, 1, 1};
    static array<function<CvrpSolution(const CvrpInstance&)>, NUM_METAHEURISTICS> functions = {
        greedyAlgorithmDefault,
        clarkeWrightSavingsDefault,
        simulatedAnnealingDefault,
        granularTabuSearchDefault,
        antColonyOptimizationDefault,
    };

    array<ofstream, NUM_METAHEURISTICS> fileStreams;
    for (size_t i = 0; i < NUM_METAHEURISTICS; ++i) {
        fileStreams[i] = ofstream(fileNames[i]);
        fileStreams[i] << csvHeader;
    }

    for (const auto& name : cvrpInstances) {
        CvrpInstance instance = loadInstance(name);

        for (size_t i = 0; i < NUM_METAHEURISTICS; ++i) {
            for (size_t iter = 0; iter < iterations[i]; ++iter) {
                auto start = high_resolution_clock::now();
                CvrpSolution solution = functions[i](instance);
                auto end = high_resolution_clock::now();
                
                printResults(fileStreams[i], name, instance, solution, interval<chrono::microseconds>(start, end));
            }
        }
    }
}

void simulatedAnnealingAnalysis() {
    static const char* name = "0-pa-34";
    static const u32 iterations = 5;

    CvrpInstance instance = loadInstance(name);
    ofstream ofs;
    SimulatedAnnealingConfig config;
    
    ofs.open("sa_trivial.csv");
    ofs << csvHeader;
    config.initialSolutionType = TRIVIAL;
    for (size_t i = 0; i < iterations; ++i) {
        auto start = high_resolution_clock::now();
        CvrpSolution solution = simulatedAnnealing(instance, config);
        auto end = high_resolution_clock::now();
        printResults(ofs, name, instance, solution, interval<chrono::microseconds>(start, end));
    }
    ofs.close();

    ofs.open("sa_greedy.csv");
    ofs << csvHeader;
    config.initialSolutionType = GREEDY;
    for (size_t i = 0; i < iterations; ++i) {
        auto start = high_resolution_clock::now();
        CvrpSolution solution = simulatedAnnealing(instance, config);
        auto end = high_resolution_clock::now();
        printResults(ofs, name, instance, solution, interval<chrono::microseconds>(start, end));
    }
    ofs.close();

    ofs.open("sa_clarke_wright.csv");
    ofs << csvHeader;
    config.initialSolutionType = CLARKE_WRIGHT;
    for (size_t i = 0; i < iterations; ++i) {
        auto start = high_resolution_clock::now();
        CvrpSolution solution = simulatedAnnealing(instance, config);
        auto end = high_resolution_clock::now();
        printResults(ofs, name, instance, solution, interval<chrono::microseconds>(start, end));
    }
    ofs.close();
}

void granularTabuSearchAnalysis() {
    static const char* name = "0-pa-34";
    static const array<double, 5> betaValues = {1.25, 1.5, 2, 2.5, 3};
    static const array<u32, 5> iterationValues = {360, 300, 225, 180, 150};

    CvrpInstance instance = loadInstance(name);
    ofstream ofs;

    ofs.open("gts_beta.csv");
    ofs << "beta," << csvHeader;
    for (double beta : betaValues) {
        auto start = high_resolution_clock::now();
        CvrpSolution solution = granularTabuSearch(instance, 300, beta);
        auto end = high_resolution_clock::now();
        ofs << beta << ",";
        printResults(ofs, name, instance, solution, interval<chrono::microseconds>(start, end));
    }
    ofs.close();

    ofs.open("gts_beta_iter.csv");
    ofs << "beta,iterations," << csvHeader;
    for (size_t i = 0; i < betaValues.size(); ++i) {
        double beta = betaValues[i];
        u32 iterations = iterationValues[i];

        auto start = high_resolution_clock::now();
        CvrpSolution solution = granularTabuSearch(instance, iterations, beta);
        auto end = high_resolution_clock::now();
        ofs << beta << "," << iterations << ",";
        printResults(ofs, name, instance, solution, interval<chrono::microseconds>(start, end));
    }
    ofs.close();
}
