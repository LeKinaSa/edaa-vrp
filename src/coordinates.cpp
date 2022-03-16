#include "coordinates.hpp"
#include <iostream>

using namespace std;

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

ostream& operator<<(ostream& os, const Coordinates& obj) {
    os << "(" << obj.getLatitude() << ", " << obj.getLongitude() << ")";
    return os;
}
