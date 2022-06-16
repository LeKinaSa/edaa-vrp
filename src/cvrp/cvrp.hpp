
#ifndef CVRP_H
#define CVRP_H

#include "../coordinates.hpp"
#include "../utils.hpp"
#include <string>
#include <vector>
#include <set>

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
        const std::vector<std::vector<double>>& getDistanceMatrix() const;

        void readDistanceMatrixFromFile(const char* path);
        void writeDistanceMatrixToFile(const char* path) const;

        double routeLength(const std::vector<u64>& route) const;
        double routeWeight(const std::vector<u64>& route) const;

        void setDistance(size_t from, size_t to, double distance);

        std::vector<std::vector<u64>> distanceOrderedDeliveries() const;
    private:
        double vehicleCapacity;
        Coordinates origin;
        std::vector<CvrpDelivery> deliveries;
        std::vector<std::vector<double>> distanceMatrix;
};

struct CvrpSolution {
    std::vector<std::vector<u64>> routes;
    double length;

    bool operator<(const CvrpSolution& other);
};

#endif // CVRP_H
