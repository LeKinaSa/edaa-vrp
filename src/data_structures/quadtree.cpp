
#include "quadtree.hpp"

using namespace std;

AABB::AABB(Coordinates center, double halfSize) : center(center),
    halfSize(halfSize) {}

bool AABB::containsPoint(Coordinates coords) {
    double distLat = abs(coords.getLatitude() - center.getLatitude()),
        distLong = abs(coords.getLongitude() - center.getLongitude());

    return distLat <= halfSize && distLong <= halfSize;
}

void AABB::split(AABB* newBoundaries[4]) {
    double splitSize = halfSize / 2;
    Coordinates* nw = new Coordinates(+splitSize, -splitSize);
    Coordinates* ne = new Coordinates(+splitSize, +splitSize);
    Coordinates* sw = new Coordinates(-splitSize, -splitSize);
    Coordinates* se = new Coordinates(-splitSize, +splitSize);
    newBoundaries[0] = new AABB(center + *nw, splitSize);
    newBoundaries[1] = new AABB(center + *ne, splitSize);
    newBoundaries[2] = new AABB(center + *sw, splitSize);
    newBoundaries[3] = new AABB(center + *se, splitSize);
}

ostream& operator<<(ostream& os, const AABB& obj) {
    os << obj.center << " - " << obj.halfSize;
    return os;
}

Quadtree::Quadtree(AABB boundary) : boundary(boundary), point(nullptr) {
}

void Quadtree::insert(pair<u64, Coordinates>& newPoint) {
    if (!(boundary.containsPoint(newPoint.second))) {
        return;
    }
    if ((nw == nullptr) && (ne == nullptr) && (sw == nullptr) && (se == nullptr)) {
        // This is a Leaf
        if (point == nullptr) {
            point = &newPoint;
            return;
        }
        // Doesn't have space
        this->subdivide();
    }

    nw->insert(newPoint);
    ne->insert(newPoint);
    sw->insert(newPoint);
    se->insert(newPoint);
}

void Quadtree::subdivide() {
    AABB* newBoundaries[4];
    boundary.split(newBoundaries);

    nw = new Quadtree(*newBoundaries[0]);
    ne = new Quadtree(*newBoundaries[1]);
    sw = new Quadtree(*newBoundaries[2]);
    se = new Quadtree(*newBoundaries[3]);

    nw->insert(*point);
    ne->insert(*point);
    sw->insert(*point);
    se->insert(*point);
    point = nullptr;
}

ostream& operator<<(ostream& os, const Quadtree& obj) {
    if (obj.nw != nullptr) {
        os << "NW [" << *(obj.nw) << "] ";
    }

    if (obj.ne != nullptr) {
        os << "NE [" << *(obj.ne) << "] ";
    }

    if (obj.point == nullptr) {
        os << "null";
    }
    else {
        os << obj.point->first << " " << obj.point->second;
    }
    os << " -> " << obj.boundary;

    if (obj.sw != nullptr) {
        os << " SW [" << *(obj.sw) << "] ";
    }

    if (obj.se != nullptr) {
        os << "SE [" << *(obj.se) << "]";
    }

    return os;
}
