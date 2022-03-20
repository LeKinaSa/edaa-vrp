
#ifndef QUADTREE_H
#define QUADTREE_H

#include "../coordinates.hpp"
#include "../types.hpp"

class AABB {
    public:
        AABB(Coordinates center, double halfSize);
        bool containsPoint(Coordinates coords);
    private:
        Coordinates center;
        double halfSize;
};

class Quadtree {
    public:
        Quadtree(AABB boundary);
        void insert(std::pair<u64, Coordinates> newPoint);
    private:
        void subdivide();    

        std::pair<u64, Coordinates>* point;
        AABB boundary;
        Quadtree* nw, * ne, * sw, * se;
};

#endif // QUADTREE_H
