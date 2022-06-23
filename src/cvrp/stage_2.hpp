
#ifndef CVRP_STAGE_2_H
#define CVRP_STAGE_2_H

#include <string>
#include "cvrp.hpp"

CvrpSolution applyCvrpAlgorithm(std::string algorithm, const CvrpInstance& instance,  bool config);

#endif // CVRP_STAGE_2_H
