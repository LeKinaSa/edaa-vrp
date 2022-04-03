
#include "cvrp.hpp"
#include <json/json.hpp>
#include <fstream>

using namespace std;
using json = nlohmann::json;

CvrpInstance::CvrpInstance(double vehicleCapacity, Coordinates origin,
        vector<CvrpDelivery> deliveries, vector<vector<double>> distanceMatrix)
        : vehicleCapacity(vehicleCapacity), origin(origin), deliveries(deliveries),
        distanceMatrix(distanceMatrix) {}

CvrpInstance::CvrpInstance(ifstream& stream) {
    auto json = json::parse(stream);

    auto originJson = json["origin"];
    origin = Coordinates(originJson["lat"], originJson["lng"]);

    vehicleCapacity = json["vehicle_capacity"];

    auto deliveriesJson = json["deliveries"];
    deliveries = vector<CvrpDelivery>(deliveriesJson.size());

    for (auto delivery : deliveriesJson.items()) {
        auto v = delivery.value();
        
        string id = v["id"];
        auto point = v["point"];
        double size = v["size"];
        deliveries.push_back({id, Coordinates(point["lat"], point["lng"]), size});
    }
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
