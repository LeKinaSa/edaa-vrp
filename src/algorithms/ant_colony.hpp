
#ifndef ANT_COLONY_H
#define ANT_COLONY_H

#include "../cvrp/cvrp.hpp"

CvrpSolution antColonyOptimization(const CvrpInstance& instance, size_t maxIterations = 1000);

#endif // ANT_COLONY_H
