
#include <json/json.hpp>
#include <fstream>
#include "cvrp.hpp"
#include "../data_structures/binary_heap.hpp"

using namespace std;
using json = nlohmann::json;

CvrpInstance::CvrpInstance(ifstream& stream) {
    auto json = json::parse(stream);

    auto originJson = json["origin"];
    origin = Coordinates(originJson["lat"], originJson["lng"]);

    vehicleCapacity = json["vehicle_capacity"];

    auto deliveriesJson = json["deliveries"];
    deliveries = vector<CvrpDelivery>();

    for (auto delivery : deliveriesJson.items()) {
        auto v = delivery.value();
        
        string id = v["id"];
        auto point = v["point"];
        double size = v["size"];
        deliveries.push_back({id, Coordinates(point["lat"], point["lng"]), size});
    }

    // Index 0 is for the depot, indices 1 to n correspond to indices 0 to n-1
    // of the deliveries vector
    size_t numDeliveries = deliveries.size();
    distanceMatrix = vector<vector<double>>(
        numDeliveries + 1, vector<double>(numDeliveries + 1, 0)
    );
}

double CvrpInstance::getVehicleCapacity() const {
    return vehicleCapacity;
}

const Coordinates& CvrpInstance::getOrigin() const {
    return origin;
}

const vector<CvrpDelivery>& CvrpInstance::getDeliveries() const {
    return deliveries;
}

const vector<vector<double>>& CvrpInstance::getDistanceMatrix() const {
    return distanceMatrix;
}

void CvrpInstance::readDistanceMatrixFromFile(const char* path) {
    ifstream ifs(path);

    for (u32 row = 0; row < distanceMatrix.size(); ++row) {
        for (u32 col = 0; col < distanceMatrix[row].size(); ++col) {
            ifs >> distanceMatrix[row][col];
        }
    }

    ifs.close();
}

void CvrpInstance::writeDistanceMatrixToFile(const char* path) const {
    ofstream ofs(path);

    for (const auto& row : distanceMatrix) {
        for (double val : row) {
            ofs << val << " ";
        }
        ofs << "\n";
    }

    ofs.close();
}

double CvrpInstance::routeLength(const vector<u64>& route) const {
    double length = 0;
    for (size_t i = 0; i < route.size() - 1; ++i) {
        length += distanceMatrix[route[i]][route[i + 1]];
    }
    return length;
}

double CvrpInstance::routeWeight(const vector<u64>& route) const {
    double weight = 0;
    for (size_t i = 1; i < route.size() - 1; ++i) {
        weight += deliveries[route[i] - 1].size;
    }
    return weight;
}

void CvrpInstance::setDistance(size_t from, size_t to, double distance) {
    if (distance > 0) {
        distanceMatrix[from][to] = distance;
    }
}

vector<vector<u64>> CvrpInstance::distanceOrderedDeliveries() const {
    vector<vector<u64>> res;
    res.reserve(distanceMatrix.size());

    for (u64 i = 0; i < distanceMatrix.size(); ++i) {
        BinaryHeap<u64> heap(distanceMatrix.size());
        vector<u64> ordered;
        ordered.reserve(distanceMatrix.size());

        // Don't include edges going back to the depot, since we only want deliveries
        for (u64 j = 1; j < distanceMatrix.size(); ++j) {
            if (i != j) {
                heap.insert(j, distanceMatrix[i][j]);
            }
        }

        while (!heap.empty()) {
            ordered.push_back(heap.extractMin());
        }
        res.push_back(ordered);
    }

    return res;
}

bool CvrpSolution::operator<(const CvrpSolution& other) {
    return length < other.length;
}
