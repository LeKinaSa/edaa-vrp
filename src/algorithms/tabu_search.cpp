
#include <list>
#include <iostream>
#include <algorithm>
#include <queue>
#include <unordered_map>
#include <functional>
#include "tabu_search.hpp"
#include "../utils.hpp"

using namespace std;

typedef pair<u64, u64> Edge;

class ClarkeWrightRoute {
    public:
        ClarkeWrightRoute(const CvrpInstance& instance) : id(nextId++),
                distanceMatrix(instance.getDistanceMatrix()), deliveries(instance.getDeliveries()),
                route({}), length(0), weight(0) {}

        ClarkeWrightRoute(const CvrpInstance& instance, const list<Edge>& route) : id(nextId++),
                distanceMatrix(instance.getDistanceMatrix()), deliveries(instance.getDeliveries()),
                route(route), length(0), weight(0) {
            for (auto it = route.begin(); it != route.end(); ++it) {
                length += distanceMatrix.get()[it->first][it->second];
                if (it->first != 0) {
                    weight += deliveries.get().at(it->first - 1).size;
                }
            }
        }

        double getLength() const {
            return length;
        }

        double getWeight() const {
            return weight;
        }

        const list<Edge>& getRoute() const {
            return route;
        }

        const Edge& front() const {
            return route.front();
        }

        const Edge& back() const {
            return route.back();
        }

        u64 firstDelivery() const {
            return route.front().second;
        }

        u64 lastDelivery() const {
            return route.back().first;
        }

        void popFront() {
            Edge edge = route.front();
            route.pop_front();
            length -= distanceMatrix.get()[edge.first][edge.second];
            if (edge.first != 0) {
                weight -= deliveries.get()[edge.first - 1].size;
            }
        }

        void popBack() {
            Edge edge = route.back();
            route.pop_back();
            length -= distanceMatrix.get()[edge.first][edge.second];
            if (edge.first != 0) {
                weight -= deliveries.get()[edge.first - 1].size;
            }
        }

        void pushBack(const Edge& edge) {
            route.push_back(edge);
            length += distanceMatrix.get()[edge.first][edge.second];
            if (edge.first != 0) {
                weight += deliveries.get()[edge.first - 1].size;
            }
        }

        void extend(const ClarkeWrightRoute& other) {
            for (auto it = other.route.begin(); it != other.route.end(); ++it) {
                pushBack(*it);
            }
        }

        vector<u64> toNodeRoute() const {
            vector<u64> nodeRoute;
            nodeRoute.reserve(route.size() + 1);

            for (auto it = route.begin(); it != route.end(); ++it) {
                nodeRoute.push_back(it->first);
            }
            nodeRoute.push_back(route.back().second);

            return nodeRoute;
        }

        bool operator==(const ClarkeWrightRoute& other) {
            return id == other.id;
        }
    private:
        static u32 nextId;

        u32 id;
        reference_wrapper<const vector<CvrpDelivery>> deliveries;
        reference_wrapper<const vector<vector<double>>> distanceMatrix;
        list<Edge> route;
        double length, weight;
};

u32 ClarkeWrightRoute::nextId = 1;

CvrpSolution clarkeWrightSavings(const CvrpInstance& instance) {
    const vector<vector<double>>& distanceMatrix = instance.getDistanceMatrix();

    list<ClarkeWrightRoute> routes;
    unordered_map<u64, reference_wrapper<ClarkeWrightRoute>> deliveryToRoute;

    auto savings = [&distanceMatrix](const Edge& e) {
        return distanceMatrix[e.first][0] + distanceMatrix[0][e.second] - distanceMatrix[e.first][e.second];
    };
    auto comp = [&savings](const Edge& e1, const Edge& e2) {
        return savings(e1) > savings( e2);
    };
    vector<Edge> savingsList;

    // Create a set of dummy routes (each delivery location is visited by a separate vehicle)
    for (size_t i = 1; i < distanceMatrix.size(); ++i) {
        routes.push_back(ClarkeWrightRoute(instance, {{0, i}, {i, 0}}));
        deliveryToRoute.emplace(i, routes.back());

        // Add edges to savings list
        for (size_t j = 1; j < distanceMatrix.size(); ++j) {
            if (i != j) {
                savingsList.push_back({i, j});
            }
        }
    }

    // Sort savings list by savings, in descending order
    sort(savingsList.begin(), savingsList.end(), comp);

    // Iterate through the savings list
    for (auto it = savingsList.begin(); it != savingsList.end(); ++it) {
        u64 start = it->first;
        u64 end = it->second;
        ClarkeWrightRoute& startRoute = deliveryToRoute.at(start);
        ClarkeWrightRoute& endRoute = deliveryToRoute.at(end);

        // Nodes are already part of the same route
        if (startRoute == endRoute) {
            continue;
        }

        // Edge nodes are internal to the route
        if ((start != startRoute.firstDelivery() && start != startRoute.lastDelivery()) ||
                (end != endRoute.firstDelivery() && end != endRoute.lastDelivery())) {
            continue;
        }

        if (start == startRoute.lastDelivery() && end == endRoute.firstDelivery()) {
            if ((startRoute.getWeight() + endRoute.getWeight()) <= instance.getVehicleCapacity()) {
                cout << "Found new saving: (" << start << ", " << end << ")" << endl;

                startRoute.popBack();
                endRoute.popFront();

                startRoute.pushBack(*it);
                startRoute.extend(endRoute);

                const list<Edge>& lst = endRoute.getRoute();
                for (const Edge& edge : lst) {
                    deliveryToRoute.at(edge.first) = startRoute;
                }

                for (auto itr = routes.begin(); itr != routes.end(); ++itr) {
                    if (*itr == endRoute) {
                        routes.erase(itr);
                        break;
                    }
                }
            }
        }
    }

    double length = 0;
    std::vector<std::vector<u64>> convertedRoutes;
    convertedRoutes.reserve(routes.size());

    for (const auto& route : routes) {
        convertedRoutes.push_back(route.toNodeRoute());
        length += route.getLength();
    }

    return { convertedRoutes, length };
}