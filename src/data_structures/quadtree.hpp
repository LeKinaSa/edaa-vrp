
#ifndef QUADTREE_H
#define QUADTREE_H

#include <memory>
#include "../coordinates.hpp"
#include "../types.hpp"
#include "../osm/osm.hpp"

class AABB {
    public:
        AABB(Coordinates center, double halfSize);
        bool containsPoint(const Coordinates& coords);
        std::array<AABB, 4> split();

        friend std::ostream& operator<<(std::ostream& os, const AABB& obj);
    private:
        Coordinates center;
        double halfSize;
};

class Quadtree {
    public:
        Quadtree(AABB boundary);
        void insert(const OsmNode& newPoint);
        const OsmNode* nearestNeighbor(const Coordinates& queryPoint) const;

        friend std::ostream& operator<<(std::ostream& os, const Quadtree& obj);
    private:
        void subdivide();

        const OsmNode* point;
        AABB boundary;
        std::unique_ptr<Quadtree> nw, ne, sw, se;
};

#endif // QUADTREE_H
