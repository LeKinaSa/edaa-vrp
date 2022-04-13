
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
        explicit CvrpInstance(std::ifstream& stream);

        double getVehicleCapacity() const;
        const Coordinates& getOrigin() const;
        const std::vector<CvrpDelivery>& getDeliveries() const;
        const std::vector<std::vector<double>> getDistanceMatrix() const;

        void readDistanceMatrixFromFile(const char* path);
        void writeDistanceMatrixToFile(const char* path) const;

        void setDistance(size_t from, size_t to, double distance);
    private:
        double vehicleCapacity;
        Coordinates origin;
        std::vector<CvrpDelivery> deliveries;
        std::vector<std::vector<double>> distanceMatrix;
};

#endif // CVRP_H
