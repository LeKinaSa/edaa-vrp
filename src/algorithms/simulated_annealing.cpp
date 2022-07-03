
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <vector>

#include "tabu_search.hpp"
#include "simulated_annealing.hpp"
#include "greedy.hpp"

using namespace std;

static const double MIN_TEMPERATURE = 1;

// Initialize random number generator
static random_device rd;
static mt19937 rng(rd());

vector<u64> convertSolution(const CvrpSolution& solution) {
    vector<u64> converted;

    for (const auto& route : solution.routes) {
        converted.insert(converted.end(), route.begin() + 1, route.end());
    }
    converted.pop_back();

    return converted;
}

vector<u64> initialSolution(const CvrpInstance& instance, InitialSolution type) {
    vector<u64> solution;

    switch (type) {
        case TRIVIAL:
            for (u64 i = 1; i <= instance.getDeliveries().size(); ++i) {
                solution.push_back(i);
                solution.push_back(0);
            }
            solution.pop_back();
            break;
        case GREEDY: {
            solution = convertSolution(greedyAlgorithm(instance));
            break;
        }
        case CLARKE_WRIGHT: {
            solution = convertSolution(clarkeWrightSavings(instance));
            break;
        }
    }

    return solution;
}

double calculateSolutionLength(const vector<vector<double>>& distanceMatrix, const std::vector<u64>& solution) {
    double length = distanceMatrix[0][solution.front()];

    for (size_t i = 1; i < solution.size(); ++i) {
        length += distanceMatrix[solution[i - 1]][solution[i]];
    }
    length += distanceMatrix[solution.back()][0];

    return length;
}

bool isValid(const CvrpInstance& instance, const std::vector<u64>& solution) {
    const vector<CvrpDelivery>& deliveries = instance.getDeliveries();
    double vehicleCapacity = instance.getVehicleCapacity(), currentWeight = 0;

    for (const auto& location : solution) {
        if (location == 0) {
            currentWeight = 0;
            continue;
        }
        currentWeight += deliveries[location - 1].size;
        if (currentWeight > vehicleCapacity) return false;
    }

    return true;
}

vector<u64> randomNeighbor(const vector<u64>& oldSolution) {
    vector<u64> newSolution = oldSolution;

    u8 heuristic = rand() % 4;
    size_t i = rand() % (newSolution.size() - 1);
    size_t j = randomIntInclusive(i + 1, newSolution.size() - 1);

    switch (heuristic) {
        case 0: // Swap
            swap(newSolution[i], newSolution[j]);
            break;
        case 1: // Scramble
            random_shuffle(newSolution.begin() + i, newSolution.begin() + j + 1);
            break;
        case 2: { // Insert
            u64 value = newSolution[i];
            newSolution.erase(newSolution.begin() + i);
            newSolution.insert(newSolution.begin() + j, value);
            break;
        }
        case 3: // Reverse
            reverse(newSolution.begin() + i, newSolution.begin() + j + 1);
            break;
    }

    return newSolution;
}

vector<u64> randomValidNeighbor(const CvrpInstance& instance, const vector<u64>& oldSolution) {
    vector<u64> neighbor;

    bool valid = false;
    while (!valid) {
        neighbor = randomNeighbor(oldSolution);
        valid = isValid(instance, neighbor);
    }

    return neighbor;
}

vector<u64> simulatedAnnealingAlgorithm(const CvrpInstance& instance, const SimulatedAnnealingConfig& config, bool printLogs) {
    srand(time(NULL));
    uniform_real_distribution dist;
    const vector<vector<double>>& distanceMatrix = instance.getDistanceMatrix();

    vector<u64> currentSolution = initialSolution(instance, config.initialSolutionType);
    double currentLength = calculateSolutionLength(distanceMatrix, currentSolution);

    vector<u64> bestSolution = currentSolution;
    double bestLength = currentLength;

    if (printLogs) cout << "Initial solution: " << currentLength / 1000.0 << endl;

    double temperature = bestLength;
    double temperatureDecrease = pow(MIN_TEMPERATURE / temperature, 1.0 / (config.numIters - 1.0));

    while (temperature > MIN_TEMPERATURE) {
        vector<u64> newSolution = randomValidNeighbor(instance, currentSolution);
        double newLength = calculateSolutionLength(distanceMatrix, newSolution);

        if (newLength < bestLength) {
            bestSolution = newSolution;
            bestLength = newLength;
            if (printLogs) cout << "New best solution: " << bestLength / 1000.0 << endl;
        }

        double delta = newLength - currentLength;
        double randomProbability = dist(rng);
        if (delta <= 0 || randomProbability <= exp(-delta / temperature)) {
            currentSolution = move(newSolution);
            currentLength = newLength;
        }

        temperature *= temperatureDecrease;
    }

    return bestSolution;
}

CvrpSolution simulatedAnnealing(const CvrpInstance& instance, SimulatedAnnealingConfig config, bool printLogs) {
    srand(time(nullptr));

    vector<u64> solution = simulatedAnnealingAlgorithm(instance, config, printLogs);

    // Normalize solution
    const vector<vector<double>>& distanceMatrix = instance.getDistanceMatrix();

    vector<vector<u64>> routes;
    double length = 0;
    vector<u64> currentRoute = {0};
    for (const auto& location : solution) {
        length += distanceMatrix[currentRoute.back()][location];
        currentRoute.push_back(location);
        if (location == 0) {
            if (currentRoute.size() > 2) routes.push_back(currentRoute);
            currentRoute = {0};
        }
    }
    length += distanceMatrix[currentRoute.back()][0];
    currentRoute.push_back(0);
    if (currentRoute.size() > 2) routes.push_back(currentRoute);

    if (printLogs) cout << "Final solution has length " << length / 1000.0 << ", uses "
        << routes.size() << " vehicles." << endl;

    return { routes, length };
}
