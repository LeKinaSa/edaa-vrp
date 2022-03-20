
#include "quadtree.hpp"

using namespace std;

AABB::AABB(Coordinates center, double halfSize) : center(center),
    halfSize(halfSize) {}

bool AABB::containsPoint(Coordinates coords) {
    double distLat = abs(coords.getLatitude() - center.getLatitude()),
        distLong = abs(coords.getLongitude() - center.getLongitude());

    return distLat <= halfSize && distLong <= halfSize;
}

Quadtree::Quadtree(AABB boundary) : boundary(boundary) {}

void Quadtree::insert(pair<u64, Coordinates> newPoint) {
    // TODO
}

void Quadtree::subdivide() {
    // TODO
}
