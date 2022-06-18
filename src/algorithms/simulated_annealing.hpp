
#ifndef SIMULATED_ANNEALING_H
#define SIMULATED_ANNEALING_H

#include "../cvrp/cvrp.hpp"

CvrpSolution simulatedAnnealing(const CvrpInstance& instance, bool greedyInitialSolution = true);

#endif // SIMULATED_ANNEALING_H
