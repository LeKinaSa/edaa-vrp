
#include <algorithm>
#include "kd_tree.hpp"

using namespace std;

KDTree::KDTree(PointVector& points) {
    if (points.empty()) {
        return;
    }

    root = buildTree(points.begin(), points.end(), true);
    size = points.size();
}

KDTree::~KDTree() {
    freeNodes(root);
}

bool KDTree::empty() const {
    return size == 0;
}

const OsmNode* KDTree::nearestNeighbor(const Coordinates& queryPoint) const {
    const Node* node = findNearest(root, queryPoint);
    if (node) {
        return &node->point;
    }
    return nullptr;
}

const KDTree::Node* KDTree::findNearest(const Node* root,
        const Coordinates& queryPoint, u32 depth) const {
    if (root == nullptr) return nullptr;
    bool latitude = depth % 2 == 0;

    double queryDimension, rootDimension;
    
    if (latitude) {
        queryDimension = queryPoint.getLatitude();
        rootDimension = root->point.coordinates.getLatitude();
    }
    else {
        queryDimension = queryPoint.getLongitude();
        rootDimension = root->point.coordinates.getLongitude();
    }

    const Node* next, * other;
    if (queryDimension < rootDimension) {
        next = root->left;
        other = root->right;
    }
    else {
        next = root->right;
        other = root->left;
    }

    const Node* best = findNearest(next, queryPoint, depth + 1);
    best = nearestPoint(best, root, queryPoint);

    double rSquared = queryPoint.squaredEuclideanDistance(best->point.coordinates),
        boundaryDist = queryDimension - rootDimension;

    if (rSquared >= boundaryDist * boundaryDist) {
        // There might be a better point in the other branch, explore it
        const Node* temp = findNearest(other, queryPoint, depth + 1);
        best = nearestPoint(best, temp, queryPoint);
    }

    return best;
}

const KDTree::Node* KDTree::nearestPoint(const Node* n1, const Node* n2,
        const Coordinates& point) const {
    if (n1 == nullptr) return n2;
    if (n2 == nullptr) return n1;

    double d1 = n1->point.coordinates.squaredEuclideanDistance(point),
        d2 = n2->point.coordinates.squaredEuclideanDistance(point);

    if (d1 < d2) return n1;
    return n2;
}

ostream& operator<<(ostream& os, const KDTree& obj) {
    obj.printNodes(os, obj.root);
    return os;
}

KDTree::Node* KDTree::buildTree(PointIterator start, PointIterator end, bool latitude) {
    if (start >= end) return nullptr;

    size_t len = end - start;
    PointIterator mid = start + len / 2;
    auto cmp = latitude ? compareLatitude : compareLongitude;
    nth_element(start, mid, end, cmp);

    Node* node = new Node{mid->get()};
    node->left = buildTree(start, mid, !latitude);
    node->right = buildTree(mid + 1, end, !latitude);

    return node;
}

void KDTree::freeNodes(Node* node) {
    if (node == nullptr) return;
    freeNodes(node->left);
    freeNodes(node->right);
    delete node;
}

void KDTree::printNodes(ostream& os, Node* node, u32 depth) const {
    if (node) {
        os << string(depth, '>') << node->point.coordinates << '\n';
        printNodes(os, node->left, depth + 1);
        printNodes(os, node->right, depth + 1);
    }
}