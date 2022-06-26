
#ifndef SIMULATED_ANNEALING_H
#define SIMULATED_ANNEALING_H

#include "../cvrp/cvrp.hpp"

enum InitialSolution {
    TRIVIAL,
    GREEDY,
    CLARKE_WRIGHT,
};

struct SimulatedAnnealingConfig {
    InitialSolution initialSolutionType = TRIVIAL;
    size_t numIters = 10'000'000;
};

CvrpSolution simulatedAnnealing(const CvrpInstance& instance, SimulatedAnnealingConfig config);

#endif // SIMULATED_ANNEALING_H
