
#include <unordered_set>
#include <list>
#include "greedy.hpp"

using namespace std;

CvrpSolution greedyAlgorithm(const CvrpInstance& instance, bool printLogs) {
    double capacity = instance.getVehicleCapacity();
    const vector<CvrpDelivery>& deliveries = instance.getDeliveries();
    const vector<vector<double>>& dm = instance.getDistanceMatrix();

    vector<vector<u64>> routes;
    double length = 0;
    
    unordered_set<u64> visited;
    vector<vector<u64>> ordered = instance.distanceOrderedDeliveries();

    double currentWeight = 0;
    vector<u64> currentRoute = {0};

    while (visited.size() < deliveries.size()) {
        auto it = ordered[currentRoute.back()].begin();
        auto end = ordered[currentRoute.back()].end();

        while (it != end && (visited.count(*it) || currentWeight + deliveries[*it - 1].size > capacity)) {
            ++it;
        }

        if (it != end) {
            length += dm[currentRoute.back()][*it];
            currentWeight += deliveries[*it - 1].size;
            currentRoute.push_back(*it);
            visited.insert(*it);
        }
        else {
            // Can't add more deliveries to this vehicle
            length += dm[currentRoute.back()][0];
            currentRoute.push_back(0);
            routes.push_back(currentRoute);
            currentRoute = {0};
            currentWeight = 0;
        }
    }
    length += dm[currentRoute.back()][0];
    currentRoute.push_back(0);
    routes.push_back(currentRoute);

    if (printLogs) cout << "Final solution has length " << length / 1000.0 << ", uses "
        << routes.size() << " vehicles." << endl;

    return { routes, length };
}
