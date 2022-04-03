
#include "cvrp.hpp"
#include <json/json.hpp>
#include <fstream>

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

const vector<vector<double>> CvrpInstance::getDistanceMatrix() const {
    return distanceMatrix;
}

void CvrpInstance::setDistance(size_t from, size_t to, double distance) {
    if (distance > 0) {
        distanceMatrix[from][to] = distance;
    }
}