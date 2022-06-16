
#include "ant_colony.hpp"
#include <unordered_set>
#include <random>
#include <limits>

#include "../utils.hpp"

using namespace std;

// Initialize random number generator
static random_device rd;
static mt19937 rng(rd());

CvrpSolution antColonyOptimization(const CvrpInstance& instance, size_t maxIterations) {
    CvrpSolution bestSolution = { {}, numeric_limits<double>::max() };

    const vector<CvrpDelivery>& deliveries = instance.getDeliveries();
    const vector<vector<double>>& distanceMatrix = instance.getDistanceMatrix();
    const double capacity = instance.getVehicleCapacity();

    uniform_real_distribution dist;
    const double alpha = 1.0;
    const double beta = 1.0;
    const double evaporationCoefficient = 0.3;
    const u32 numAnts = 250;

    vector<vector<double>> pheromones(distanceMatrix.size());
    for (auto& row : pheromones) {
        row.resize(distanceMatrix.size());
        fill(row.begin(), row.end(), 1.0);
    }

    auto transitionValue = [&distanceMatrix, &pheromones, alpha, beta](u64 i, u64 j) {
        return pow(pheromones[i][j], alpha) * pow(1 / distanceMatrix[i][j], beta);
    };

    auto generateAntSolution = [&deliveries, &distanceMatrix, &dist, &transitionValue, capacity]() {
        unordered_set<u64> visited;
        vector<vector<u64>> routes;
        vector<u64> currentRoute = {0};
        double length = 0, currentWeight = 0;

        auto isNodeValid = [&deliveries, &visited, &currentRoute, capacity, currentWeight](u64 i) {
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

    for (size_t iter = 0; iter < maxIterations; ++iter) {
        vector<CvrpSolution> solutions;
        solutions.reserve(numAnts);
        for (u32 ant = 0; ant < numAnts; ++ant)
            solutions.push_back(generateAntSolution());

        for (auto& row : pheromones) {
            for (auto& elem : row) {
                elem *= 1 - evaporationCoefficient;
            }
        }

        for (const auto& solution : solutions) {
            if (solution.length < bestSolution.length) {
                bestSolution = solution;
                cout << "New best solution: " << solution.length / 1000 << "km." << endl;
            }
            for (const auto& route : solution.routes) {
                for (size_t idx = 0; idx < route.size() - 1; ++idx) {
                    u64 i = route[idx], j = route[idx + 1];
                    pheromones[i][j] += 1 / solution.length;
                }
            }
        }
    }

    return bestSolution;
}
