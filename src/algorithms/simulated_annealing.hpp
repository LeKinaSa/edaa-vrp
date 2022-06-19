
#ifndef SIMULATED_ANNEALING_H
#define SIMULATED_ANNEALING_H

#include "../cvrp/cvrp.hpp"

enum InitialSolution {
    TRIVIAL,
    GREEDY,
    CLARKE_WRIGHT,
};

CvrpSolution simulatedAnnealing(const CvrpInstance& instance, InitialSolution initialSolutionType = InitialSolution::GREEDY);

#endif // SIMULATED_ANNEALING_H
