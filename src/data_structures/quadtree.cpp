
#include "quadtree.hpp"

AABB::AABB(Coordinates center, double halfSize) : center(center),
    halfSize(halfSize) {}

bool AABB::containsPoint(Coordinates coords) {
    double distLat = abs(coords.getLatitude() - center.getLatitude()),
        distLong = abs(coords.getLongitude() - center.getLongitude());

    return distLat <= halfSize && distLong <= halfSize;
}
