
#include <stack>
#include <cfloat>
#include "quadtree.hpp"

using namespace std;

AABB::AABB(Coordinates center, double halfSize) : center(center),
    halfSize(halfSize) {}

bool AABB::containsPoint(const Coordinates& coords) const {
    double distLat = abs(coords.getLatitude() - center.getLatitude()),
        distLong = abs(coords.getLongitude() - center.getLongitude());

    return distLat <= halfSize && distLong <= halfSize;
}

array<AABB, 4> AABB::split() {
    double splitSize = halfSize / 2;

    Coordinates nw(+splitSize, -splitSize),
            ne(+splitSize, +splitSize),
            sw(-splitSize, -splitSize),
            se(-splitSize, +splitSize);

    return {
        AABB(center + nw, splitSize),
        AABB(center + ne, splitSize),
        AABB(center + sw, splitSize),
        AABB(center + se, splitSize)
    };
}

ostream& operator<<(ostream& os, const AABB& obj) {
    os << obj.center << " - " << obj.halfSize;
    return os;
}

Quadtree::Quadtree(AABB boundary) : boundary(boundary), point(nullptr) {}

void Quadtree::insert(const OsmNode& newPoint) {
    if (!boundary.containsPoint(newPoint.coordinates)) {
        return;
    }

    if (nw == nullptr) {
        // Leaf node
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
    array<AABB, 4> newBoundaries = boundary.split();

    nw = make_unique<Quadtree>(newBoundaries[0]);
    ne = make_unique<Quadtree>(newBoundaries[1]);
    sw = make_unique<Quadtree>(newBoundaries[2]);
    se = make_unique<Quadtree>(newBoundaries[3]);

    nw->insert(*point);
    ne->insert(*point);
    sw->insert(*point);
    se->insert(*point);

    point = nullptr;
}

const OsmNode* Quadtree::nearestNeighbor(const Coordinates& queryPoint) const {
    stack<const Quadtree*> s;
    double minDistance = DBL_MAX;
    const OsmNode* nearest = this->point;

    s.push(this);

    while (!s.empty()) {
        const Quadtree* next = s.top(); s.pop();

        if (next->nw) {
            
        }
    }
}

ostream& operator<<(ostream& os, const Quadtree& obj) {
    if (obj.nw) {
        os << "NW [" << *(obj.nw) << "] ";
    }

    if (obj.ne) {
        os << "NE [" << *(obj.ne) << "] ";
    }

    if (obj.point == nullptr) {
        os << "null";
    }
    else {
        os << obj.point->coordinates;
    }
    os << " -> " << obj.boundary;

    if (obj.sw) {
        os << " SW [" << *(obj.sw) << "] ";
    }

    if (obj.se) {
        os << "SE [" << *(obj.se) << "]";
    }

    return os;
}

const Quadtree* Quadtree::selectQuadrant(const Coordinates& queryPoint) {
    if (nw == nullptr) {
        return this;
    }

    
}

const OsmNode* Quadtree::findNearest(const Coordinates& queryPoint,
        const OsmNode* best = nullptr, double radius = DBL_MAX) const {
    if (best == nullptr) {
        // Haven't found a point yet, recurse into the tree
        
    }

    return best;
}
