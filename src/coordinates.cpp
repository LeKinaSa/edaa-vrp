#include "coordinates.hpp"
#include "utils.hpp"
#include <iostream>
#include <cmath>

using namespace std;

Coordinates::Coordinates() : Coordinates(0.0, 0.0) {}

Coordinates::Coordinates(double latitude, double longitude)
    : latitude(latitude), longitude(longitude) {}

double Coordinates::getLatitude() const {
    return latitude;
}

double Coordinates::getLongitude() const {
    return longitude;
}

void Coordinates::setLatitude(double latitude) {
    this->latitude = latitude;
}

void Coordinates::setLongitude(double longitude) {
    this->longitude = longitude;
}

double Coordinates::haversine(const Coordinates& other) const {
    static const double EARTH_RADIUS = 6371000.0;

    double distLat = degToRad(latitude - other.latitude),
        distLong = degToRad(longitude - other.longitude);
    
    double lat1 = degToRad(latitude);
    double lat2 = degToRad(other.latitude);

    double a = pow(sin(distLat / 2), 2) + pow(sin(distLong / 2), 2) *
        cos(lat1) * cos(lat2);
    
    return EARTH_RADIUS * 2 * asin(sqrt(a));
}

ostream& operator<<(ostream& os, const Coordinates& obj) {
    os << "(" << obj.getLatitude() << ", " << obj.getLongitude() << ")";
    return os;
}

Coordinates Coordinates::operator+(const Coordinates& obj) {
    return Coordinates(latitude + obj.latitude, longitude + obj.longitude);
}
