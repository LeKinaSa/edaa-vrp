
#include <unordered_set>
#include <random>
#include <limits>
#include <set>

#include "ant_colony.hpp"
#include "../utils.hpp"

using namespace std;

// Initialize random number generator
static random_device rd;
static mt19937 rng(rd());

CvrpSolution antColonyOptimization(const CvrpInstance& instance, AntColonyConfig config) {
    CvrpSolution bestSolution = { {}, numeric_limits<double>::max() };

    const vector<CvrpDelivery>& deliveries = instance.getDeliveries();
    const vector<vector<double>>& distanceMatrix = instance.getDistanceMatrix();
    const double capacity = instance.getVehicleCapacity();

    uniform_real_distribution dist;
    const double evaporationCoefficient = 0.25;

    vector<vector<double>> pheromones(distanceMatrix.size());
    for (auto& row : pheromones) {
        row.resize(distanceMatrix.size());
        fill(row.begin(), row.end(), 1.0);
    }

    auto transitionValue = [&distanceMatrix, &pheromones, &config](u64 i, u64 j) {
        return pow(pheromones[i][j], config.alpha) * pow(1 / distanceMatrix[i][j], config.beta);
    };

    auto generateAntSolution = [&deliveries, &distanceMatrix, &dist, &transitionValue, capacity]() {
        unordered_set<u64> visited;
        vector<vector<u64>> routes;
        vector<u64> currentRoute = {0};
        double length = 0, currentWeight = 0;

        auto isNodeValid = [&deliveries, &visited, &currentRoute, &capacity, &currentWeight](u64 i) {
            return currentRoute.back() != i &&
                !visited.count(i) &&
                (i == 0 || currentWeight + deliveries[i - 1].size <= capacity);
        };

        while (visited.size() < deliveries.size()) {
            double sumProb = 0;
            bool validDelivery = false;
            for (u64 i = 0; i < distanceMatrix.size(); ++i) {
                if (isNodeValid(i)) {
                    sumProb += transitionValue(currentRoute.back(), i);
                    if (i != 0) validDelivery = true;
                }
            }

            if (!validDelivery) {
                // No valid nodes, move on to the next route
                length += distanceMatrix[currentRoute.back()][0];
                currentRoute.push_back(0);
                routes.push_back(currentRoute);
                currentRoute = {0};
                currentWeight = 0;
                continue;
            }

            u64 picked = 0;
            double r = dist(rng);
            for (u64 i = 0; i < distanceMatrix.size(); ++i) {
                if (isNodeValid(i)) {
                    double prob = transitionValue(currentRoute.back(), i) / sumProb;
                    if (r <= prob) {
                        picked = i;
                        break;
                    }
                    r -= prob;
                }
            }

            length += distanceMatrix[currentRoute.back()][picked];
            currentRoute.push_back(picked);

            if (picked == 0) {
                // Ant decided to end the route early (vehicle still has space left)
                routes.push_back(currentRoute);
                currentRoute = {0};
                currentWeight = 0;
            }
            else {
                visited.insert(picked);
                currentWeight += deliveries[picked - 1].size;
            }
        }

        return CvrpSolution { routes, length };
    };

    auto improveRoutes = [&distanceMatrix](CvrpSolution& solution, u32 numIters = 5) {
        for (size_t _ = 0; _ < numIters; ++_) {
            for (auto& route : solution.routes) {
                bool improvedRoute = false;

                for (size_t idxI = 1; !improvedRoute && idxI < route.size() - 2; ++idxI) {
                    u64 i = route[idxI], beforeI = route[idxI - 1], afterI = route[idxI + 1];
                    for (size_t idxJ = idxI + 1; !improvedRoute && idxJ < route.size() - 1; ++idxJ) {
                        u64 j = route[idxJ], beforeJ = route[idxJ - 1], afterJ = route[idxJ + 1];

                        double delta = distanceMatrix[beforeI][j] + distanceMatrix[i][afterJ] -
                            distanceMatrix[beforeI][i] - distanceMatrix[j][afterJ];

                        if (idxJ - idxI == 1) {
                            // Deliveries are adjacent
                            delta += distanceMatrix[j][i] - distanceMatrix[i][j];
                        }
                        else {
                            // Deliveries are not adjacent
                            delta += distanceMatrix[j][afterI] + distanceMatrix[beforeJ][i] -
                                distanceMatrix[i][afterI] - distanceMatrix[beforeJ][j];
                        }

                        if (delta < 0) {
                            swap(route[idxI], route[idxJ]);
                            solution.length += delta;
                            improvedRoute = true;
                        }
                    }
                }
            }
        }
    };

    vector<CvrpSolution> solutions;
    solutions.resize(config.numAnts);

    for (size_t iter = 0; iter < config.maxIterations; ++iter) {
        for (u32 ant = 0; ant < config.numAnts; ++ant)
            solutions[ant] = generateAntSolution();

        for (auto& row : pheromones) {
            for (auto& elem : row) {
                elem *= 1 - evaporationCoefficient;
            }
        }

        set<CvrpSolution> eliteSolutions;
        for (auto& solution : solutions) {
            if (config.useSwapHeuristic) improveRoutes(solution);

            if (solution.length < bestSolution.length) {
                bestSolution = solution;
            }

            if (config.eliteAnts == 0) {
                for (const auto& route : solution.routes) {
                    for (size_t idx = 0; idx < route.size() - 1; ++idx) {
                        u64 i = route[idx], j = route[idx + 1];
                        pheromones[i][j] += 1 / solution.length;
                    }
                }
            }
            else {
                if (eliteSolutions.size() < config.eliteAnts) {
                    eliteSolutions.insert(solution);
                }
                else if (!eliteSolutions.empty() && solution.length < eliteSolutions.rbegin()->length) {
                    auto it = eliteSolutions.end(); --it;
                    eliteSolutions.erase(it);
                    eliteSolutions.insert(solution);
                }
            }
        }

        if (config.eliteAnts > 0) {
            for (const auto& eliteSolution : eliteSolutions) {
                for (const auto& route : eliteSolution.routes) {
                    for (size_t idx = 0; idx < route.size() - 1; ++idx) {
                        u64 i = route[idx], j = route[idx + 1];
                        pheromones[i][j] += 1 / eliteSolution.length;
                    }
                }
            }
        }
    }

    return bestSolution;
}
