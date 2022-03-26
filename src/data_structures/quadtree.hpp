
#ifndef QUADTREE_H
#define QUADTREE_H

#include <memory>
#include "../coordinates.hpp"
#include "../types.hpp"

class AABB {
    public:
        AABB(Coordinates center, double halfSize);
        bool containsPoint(Coordinates coords);
        std::array<AABB, 4> split();

        friend std::ostream& operator<<(std::ostream& os, const AABB& obj);
    private:
        Coordinates center;
        double halfSize;
};

class Quadtree {
    public:
        Quadtree(AABB boundary);
        void insert(std::pair<u64, Coordinates> newPoint);

        friend std::ostream& operator<<(std::ostream& os, const Quadtree& obj);
    private:
        void subdivide();

        std::shared_ptr<std::pair<u64, Coordinates>> point;
        AABB boundary;
        std::unique_ptr<Quadtree> nw, ne, sw, se;
};

#endif // QUADTREE_H
