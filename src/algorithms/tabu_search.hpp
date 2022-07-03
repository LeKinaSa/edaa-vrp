
#ifndef TABU_SEARCH_H
#define TABU_SEARCH_H

#include "../cvrp/cvrp.hpp"

CvrpSolution clarkeWrightSavings(const CvrpInstance& instance, bool printLogs = false);

CvrpSolution granularTabuSearch(const CvrpInstance& instance, size_t maxIterations = 1000, double beta = 1.5, bool printLogs = false);

#endif // TABU_SEARCH_H