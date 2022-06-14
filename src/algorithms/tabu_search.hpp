
#ifndef TABU_SEARCH_H
#define TABU_SEARCH_H

#include "../cvrp/cvrp.hpp"

CvrpSolution clarkeWrightSavings(const CvrpInstance& instance);

CvrpSolution granularTabuSearch(const CvrpInstance& instance, size_t maxIterations = 1000);

#endif // TABU_SEARCH_H