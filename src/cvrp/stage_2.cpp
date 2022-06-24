
#include <functional>

#include "stage_2.hpp"
#include "../algorithms/ant_colony.hpp"
#include "../algorithms/greedy.hpp"
#include "../algorithms/simulated_annealing.hpp"
#include "../algorithms/tabu_search.hpp"

using namespace std;

template<typename T>
void readOption(T& option, const char* prompt, function<T(const string&)> conversionFunc) {
    cout << prompt << flush;
    string str;
    getline(cin, str);

    if (!str.empty()) {
        option = conversionFunc(str);
    }
}

inline u64 convertUnsignedInt(const string& str) {
    return stoull(str);
}

inline double convertDouble(const string& str) {
    return stod(str);
}

bool convertBool(const string& str) {
    string lower(str);
    transform(str.begin(), str.end(), lower.begin(), ::tolower);
    return lower == "yes";
}

CvrpSolution applyAntColonyOptimization(const CvrpInstance& instance, bool config) {
    AntColonyConfig acoConfig;

    if (config) {
        readOption<double>(acoConfig.alpha, "Alpha: ", convertDouble);
        readOption<double>(acoConfig.beta, "Beta: ", convertDouble);
        readOption<u64>(acoConfig.numAnts, "Num ants: ", convertUnsignedInt);
        readOption<u32>(acoConfig.eliteAnts, "Elite ants: ", convertUnsignedInt);
        readOption<u64>(acoConfig.maxIterations, "Max. iterations: ", convertUnsignedInt);
        readOption<bool>(acoConfig.useSwapHeuristic, "Use swap heuristic (yes / no): ", convertBool);
    }

    return antColonyOptimization(instance, acoConfig);
}

CvrpSolution applyClarkeWrightSavings(const CvrpInstance& instance, bool config) {
    return clarkeWrightSavings(instance);
}

CvrpSolution applyGranularTabuSearch(const CvrpInstance& instance, bool config) {
    size_t maxIterations = 1000;
    double beta = 1.5;

    if (config) {
        readOption<u64>(maxIterations, "Max. iterations: ", convertUnsignedInt);
        readOption<double>(beta, "Beta: ", convertDouble);
    }

    return granularTabuSearch(instance, maxIterations, beta);
}

CvrpSolution applyGreedyAlgorithm(const CvrpInstance& instance, bool config) {
    return greedyAlgorithm(instance);
}

CvrpSolution applySimulatedAnnealing(const CvrpInstance& instance, bool config) {
    InitialSolution initialSolution;

    if (config) {
        readOption<InitialSolution>(
            initialSolution,
            "Initial solution (0 - trivial, 1 - greedy, 2 - Clarke-Wright): ",
            [](const string& str) { return (InitialSolution) stoi(str); }
        );
    }

    return simulatedAnnealing(instance, initialSolution);
}

CvrpSolution applyCvrpAlgorithm(string algorithm, const CvrpInstance& instance, bool config) {
    static const unordered_map<string, function<CvrpSolution(const CvrpInstance&, bool)>> algorithms = {
        {"aco", applyAntColonyOptimization},
        {"cws", applyClarkeWrightSavings},
        {"gts", applyGranularTabuSearch},
        {"greedy", applyGreedyAlgorithm},
        {"sa", applySimulatedAnnealing}
    };

    if (algorithms.count(algorithm.c_str())) {
        return algorithms.at(algorithm.c_str())(instance, config);
    }

    return { {} , 0 };
}
