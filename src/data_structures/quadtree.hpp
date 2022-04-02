
#ifndef QUADTREE_H
#define QUADTREE_H

#include <memory>
#include <cfloat>
#include "../coordinates.hpp"
#include "../types.hpp"
#include "../osm/osm.hpp"

class AABB {
    public:
        AABB(Coordinates topLeft, Coordinates bottomRight);
        Coordinates center() const;
        double maxDimension() const;
        bool containsPoint(const Coordinates& coords) const;
        bool quadIntersects(const Coordinates& center, double radius) const;
        std::array<AABB, 4> split() const;

        friend std::ostream& operator<<(std::ostream& os, const AABB& obj);
    private:
        Coordinates topLeft, bottomRight;
};

class Quadtree {
    public:
        Quadtree(AABB boundary);
        ~Quadtree();
        void insert(const OsmNode& newPoint);
        const OsmNode* nearestNeighbor(const Coordinates& queryPoint) const;

        friend std::ostream& operator<<(std::ostream& os, const Quadtree& obj);
    private:
        struct NNResult {
            const OsmNode* point = nullptr;
            double distance = 0;
        };

        void subdivide();
        const Quadtree* selectQuadrant(const Coordinates& queryPoint) const;
        void findNearest(const Coordinates& queryPoint, NNResult& best) const;

        const OsmNode* point;
        AABB boundary;
        Quadtree* nw = nullptr;
        Quadtree* ne = nullptr; 
        Quadtree* sw = nullptr;
        Quadtree* se = nullptr;
};

#endif // QUADTREE_H
