
#ifndef QUADTREE_H
#define QUADTREE_H

#include "coordinates.hpp"

class AABB {
    public:
        AABB(Coordinates center, double halfSize);
        bool containsPoint(Coordinates coords);
    private:
        Coordinates center;
        double halfSize;
};

class Quadtree {
    // TODO
};

#endif // QUADTREE_H
