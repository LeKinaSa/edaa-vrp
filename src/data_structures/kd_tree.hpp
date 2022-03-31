
#ifndef KD_TREE_H
#define KD_TREE_H

#include <memory>
#include <vector>
#include "../types.hpp"
#include "../coordinates.hpp"
#include "../osm/osm.hpp"

// Actually a 2-d tree given the nature of the project
class KDTree {
    using PointVector = std::vector<std::reference_wrapper<OsmNode>>;
    using PointIterator = PointVector::iterator;

    public:
        explicit KDTree(PointVector& points);
        ~KDTree();

        bool empty() const;
        const OsmNode* nearestNeighbor(const Coordinates& queryPoint) const;

        friend std::ostream& operator<<(std::ostream& os, const KDTree& obj);
    private:
        struct Node {
            const OsmNode& point;
            Node* left = nullptr;
            Node* right = nullptr;
        };

        Node* root;
        u64 size;

        void freeNodes(Node* node);
        void printNodes(std::ostream& os, Node* node, u32 depth = 0) const;
        Node* buildTree(PointIterator start, PointIterator end, bool latitude);
        const Node* findNearest(const Node* root, const Coordinates& queryPoint, u32 depth = 0) const;
        const Node* nearestPoint(const Node* n1, const Node* n2, const Coordinates& point) const;

        static bool compareLatitude(std::reference_wrapper<OsmNode> p1, std::reference_wrapper<OsmNode> p2) {
            return p1.get().coordinates.getLatitude() < p2.get().coordinates.getLatitude();
        }
        static bool compareLongitude(std::reference_wrapper<OsmNode> p1, std::reference_wrapper<OsmNode> p2) {
            return p1.get().coordinates.getLongitude() < p2.get().coordinates.getLongitude();
        }
};

#endif // KD_TREE_H