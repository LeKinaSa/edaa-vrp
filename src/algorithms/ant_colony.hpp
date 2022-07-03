
#ifndef ANT_COLONY_H
#define ANT_COLONY_H

#include "../cvrp/cvrp.hpp"

struct AntColonyConfig {
    size_t maxIterations = 250;
    size_t numAnts = 100;
    double alpha = 1.0, beta = 3.0;
    u32 eliteAnts = 0;
    bool useSwapHeuristic = true;
};

CvrpSolution antColonyOptimization(const CvrpInstance& instance, AntColonyConfig config, bool printLogs = false);

#endif // ANT_COLONY_H
