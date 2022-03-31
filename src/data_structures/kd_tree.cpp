
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