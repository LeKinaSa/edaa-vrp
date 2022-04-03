
#ifndef CVRP_H
#define CVRP_H

#include "../coordinates.hpp"
#include <string>
#include <vector>

struct CvrpDelivery {
    std::string id;
    Coordinates coordinates;
    double size;
};

class CvrpInstance {
    public:
        CvrpInstance(double vehicleCapacity, Coordinates origin,
            std::vector<CvrpDelivery> deliveries,
            std::vector<std::vector<double>> distanceMatrix);
        CvrpInstance(std::ifstream& stream);

        double getVehicleCapacity() const;
        const Coordinates& getOrigin() const;
        const std::vector<CvrpDelivery>& getDeliveries() const;
        const std::vector<std::vector<double>> getDistanceMatrix() const;
    private:
        double vehicleCapacity;
        Coordinates origin;
        std::vector<CvrpDelivery> deliveries;
        std::vector<std::vector<double>> distanceMatrix;
};

#endif // CVRP_H
