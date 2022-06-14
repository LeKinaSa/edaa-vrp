
#include <unordered_set>
#include <list>
#include "greedy.hpp"

using namespace std;

CvrpSolution greedyAlgorithm(const CvrpInstance& instance) {
    double cost = 0;
    vector<vector<u64>> paths;

    const vector<CvrpDelivery>& deliveries = instance.getDeliveries();
    double vehicleCapacity = instance.getVehicleCapacity();
    const vector<vector<double>>& dm = instance.getDistanceMatrix();
    unordered_set<u64> visited;
    vector<list<u64>> ordered = instance.distanceOrderedPoints();

    double currentCost = 0;
    vector<u64> currentPath = {0};
    while (visited.size() < deliveries.size()) {
        auto it = ordered[currentPath.back()].begin();
        auto end = ordered[currentPath.back()].end();

        while (it != end && deliveries[*it].size > (vehicleCapacity - currentCost)) {
            ++it;
        }

        if (it != end) {
            currentCost += dm[currentPath.back()][*it];
            currentPath.push_back(*it);
        }
        else if (paths.size() < instance.)
    }

    return {paths, cost};
}
