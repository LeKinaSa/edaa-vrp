
#include <list>
#include <iostream>
#include <algorithm>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <optional>
#include <functional>
#include <random>

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

u32 ClarkeWrightRoute::nextId = 0;

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


struct TabuSearchRoute {
    vector<u64> route;
    double length = 0, weight = 0;

    unordered_set<Edge, PairHash> addedEdges = {}, removedEdges = {};

    void removeDelivery(const CvrpInstance& instance, size_t idx) {
        const auto& dm = instance.getDistanceMatrix();
        u64 element = route[idx], before = route[idx - 1], after = route[idx + 1];

        weight -= instance.getDeliveries().at(element - 1).size;
        length += dm[before][after] - (dm[before][element] + dm[element][after]);
        route.erase(route.begin() + idx);

        addedEdges.insert({before, after});
        removedEdges.insert({before, element});
        removedEdges.insert({element, after});
    }

    void addDelivery(const CvrpInstance& instance, size_t idx, u64 delivery) {
        const auto& dm = instance.getDistanceMatrix();
        u64 element = route[idx], before = route[idx - 1];

        weight += instance.getDeliveries().at(delivery - 1).size;
        length += dm[before][delivery] + dm[delivery][element] - dm[before][element];
        route.insert(route.begin() + idx, delivery);

        addedEdges.insert({before, delivery});
        addedEdges.insert({delivery, element});
        removedEdges.insert({before, element});
    }

    double excessLoad(const CvrpInstance& instance) const {
        return max(0.0, weight - instance.getVehicleCapacity());
    }

    void clearEdgeSets() {
        addedEdges.clear();
        removedEdges.clear();
    }
};

struct TabuSearchEdge {
    u64 idxA, idxB;
    TabuSearchRoute& tsrA;
    TabuSearchRoute& tsrB;
};

struct TabuSearchSolution {
    vector<TabuSearchRoute> routes;
    double length = 0;

    double excessLoad(const CvrpInstance& instance) const {
        double sum = 0;
        for (const auto& route : routes) {
            sum += route.excessLoad(instance);
        }
        return sum;
    }

    bool isValid(const CvrpInstance& instance) const {
        return excessLoad(instance) == 0;
    }

    double lengthWithPenalty(const CvrpInstance& instance, double penalty) const {
        return length + excessLoad(instance) * penalty;
    }

    CvrpSolution toStandardForm() const {
        vector<vector<u64>> standardRoutes;
        for (const auto& tsr : routes) {
            standardRoutes.push_back(tsr.route);
        }
        return { standardRoutes, length };
    }
};

void customerInsertion(const CvrpInstance& instance, TabuSearchSolution& solution, TabuSearchEdge& edge) {
    u64 a = edge.tsrA.route[edge.idxA];
    double lengthDelta = -(edge.tsrA.length + edge.tsrB.length);

    edge.tsrA.removeDelivery(instance, edge.idxA);
    edge.tsrB.addDelivery(instance, edge.idxB, a);

    lengthDelta += edge.tsrA.length + edge.tsrB.length;
    solution.length += lengthDelta;
}

void twoCustomer(const CvrpInstance& instance, TabuSearchSolution& solution, TabuSearchEdge& edge) {
    u64 b = edge.tsrB.route[edge.idxB], afterB = edge.tsrB.route[edge.idxB + 1];
    double lengthDelta = -(edge.tsrA.length + edge.tsrB.length);

    edge.tsrB.removeDelivery(instance, edge.idxB + 1);
    edge.tsrB.removeDelivery(instance, edge.idxB);
    edge.tsrA.addDelivery(instance, edge.idxA + 1, afterB);
    edge.tsrA.addDelivery(instance, edge.idxA + 1, b);

    lengthDelta += edge.tsrA.length + edge.tsrB.length;
    solution.length += lengthDelta;
}

void customerSwap(const CvrpInstance& instance, TabuSearchSolution& solution, TabuSearchEdge& edge) {
    u64 b = edge.tsrB.route[edge.idxB], afterA = edge.tsrA.route[edge.idxA + 1];
    double lengthDelta = -(edge.tsrA.length + edge.tsrB.length);

    edge.tsrA.removeDelivery(instance, edge.idxA + 1);
    edge.tsrB.removeDelivery(instance, edge.idxB);
    edge.tsrA.addDelivery(instance, edge.idxA + 1, b);
    edge.tsrB.addDelivery(instance, edge.idxB, afterA);

    lengthDelta += edge.tsrA.length + edge.tsrB.length;
    solution.length += lengthDelta;
}

// Initialize random number generator
static random_device rd;
static mt19937 rng(rd());

static const u32 MIN_PENALTY = 1, MAX_PENALTY = 6400, INITIAL_PENALTY = 100,
    PENALTY_UPDATE_ITERS = 10;

CvrpSolution granularTabuSearch(const CvrpInstance& instance, size_t maxIterations) {
    typedef function<void(const CvrpInstance&, TabuSearchSolution&, TabuSearchEdge&)> TabuSearchHeuristic;

    CvrpSolution initialSolution = clarkeWrightSavings(instance);
    const vector<vector<double>>& distanceMatrix = instance.getDistanceMatrix();

    double beta = 1.5;
    auto isShort = [&beta, &instance, &initialSolution](const Edge& edge) {
        double maxLength = beta * initialSolution.length / (instance.getDeliveries().size() + initialSolution.routes.size());
        double edgeLength = instance.getDistanceMatrix()[edge.first][edge.second];
        return edgeLength <= maxLength;
    };

    TabuSearchSolution bestSolution, currentSolution;
    for (const auto& route : initialSolution.routes) {
        double routeLength = instance.routeLength(route);
        bestSolution.routes.push_back({route, routeLength, instance.routeWeight(route)});
        bestSolution.length += routeLength;
    }
    currentSolution = bestSolution;

    cout << "Clarke-Wright initial solution has length " << bestSolution.length / 1000 << "km" << endl;

    uniform_int_distribution<int> tenureDistribution(5, 10);
    unordered_map<Edge, u8, PairHash> tabuList;
    u32 penalty = INITIAL_PENALTY, valid = 0;

    for (size_t iter = 1; iter <= maxIterations; ++iter) {
        optional<TabuSearchSolution> iterationBest;
        unordered_set<Edge, PairHash> removedEdges;
        u32 movesEvaluated = 0;

        for (size_t ra = 0; ra < currentSolution.routes.size(); ++ra) {
            for (size_t rb = 0; rb < currentSolution.routes.size(); ++rb) {
                if (ra != rb) {
                    TabuSearchRoute& tsrA = currentSolution.routes[ra];
                    TabuSearchRoute& tsrB = currentSolution.routes[rb];

                    for (size_t idxA = 1; idxA < tsrA.route.size() - 1; ++idxA) {
                        for (size_t idxB = 1; idxB < tsrB.route.size() - 1; ++idxB) {
                            if (isShort({tsrA.route[idxA], tsrB.route[idxB]})) {
                                auto applyHeuristic = [&instance, &tabuList, &penalty, &iterationBest, &removedEdges,
                                        &bestSolution, &currentSolution, &movesEvaluated, ra, rb, idxA, idxB]
                                        (const TabuSearchHeuristic& heuristic) {
                                    TabuSearchSolution newSolution = currentSolution;
                                    TabuSearchEdge edge = {idxA, idxB, newSolution.routes[ra], newSolution.routes[rb]};
                                    edge.tsrA.clearEdgeSets();
                                    edge.tsrB.clearEdgeSets();
                                    heuristic(instance, newSolution, edge);

                                    bool tabu = false;
                                    unordered_set<Edge, PairHash> addedEdges = edge.tsrA.addedEdges;
                                    addedEdges.insert(edge.tsrB.addedEdges.begin(), edge.tsrB.addedEdges.end());
                                    for (const Edge& added : addedEdges) {
                                        if (tabuList.count(added)) {
                                            tabu = true;
                                            break;
                                        }
                                    }

                                    if (newSolution.length < bestSolution.length && newSolution.isValid(instance)) {
                                        bestSolution = newSolution;
                                    }

                                    if (!tabu) {
                                        if (!iterationBest.has_value() || newSolution.lengthWithPenalty(instance, penalty) < iterationBest->lengthWithPenalty(instance, penalty)) {
                                            iterationBest = move(newSolution);
                                        }
                                        ++movesEvaluated;
                                    }
                                };

                                applyHeuristic(customerInsertion);
                                if (idxB < tsrB.route.size() - 2) {
                                    applyHeuristic(twoCustomer);
                                }
                                if (idxA < tsrA.route.size() - 2) {
                                    applyHeuristic(customerSwap);
                                }
                            }
                        }
                    }
                }
            }
        }

        if (iterationBest->isValid(instance)) ++valid;

        for (auto& route : iterationBest->routes) {
            removedEdges.insert(route.removedEdges.begin(), route.removedEdges.end());
            route.clearEdgeSets();
        }

        for (auto it = tabuList.cbegin(); it != tabuList.cend();) {
            tabuList[it->first] -= 1;
            if (tabuList[it->first] == 0) {
                it = tabuList.erase(it);
            }
            else {
                ++it;
            }
        }

        for (const Edge& edge : removedEdges) {
            tabuList[edge] = tenureDistribution(rng);
        }

        currentSolution = move(*iterationBest);

        if (iter % PENALTY_UPDATE_ITERS == 0) {
            if (valid == PENALTY_UPDATE_ITERS) {
                // All valid
                penalty = max(MIN_PENALTY, penalty / 2);
            }
            else if (valid == 0) {
                // All invalid
                penalty = min(MAX_PENALTY, penalty * 2);
            }
            valid = 0;
        }
    }

    return bestSolution.toStandardForm();
}
