
#include <array>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <random>
#include "complexity.hpp"
#include "../data_structures/kd_tree.hpp"
#include "../data_structures/quadtree.hpp"
#include "../data_structures/fibonacci_heap.hpp"

using namespace std;
using chrono::high_resolution_clock;
using chrono::duration_cast;
using chrono::microseconds;
using chrono::nanoseconds;

double randomDouble(double min, double max) {
    return min + static_cast<double>(rand()) / static_cast<double>(RAND_MAX / (max - min));
}

template <typename T>
inline u64 interval(const high_resolution_clock::time_point& start,
        const high_resolution_clock::time_point& end) {
    return duration_cast<T>(end - start).count();
}

void kdTreeQuadtreeComplexityAnalysis(u32 seed) {
    static const size_t size = 10;
    static const double minC = -75, maxC = 75;
    static const array<u32, size> numPoints = {100, 500, 1000, 2500, 5000, 10000,
        50000, 100000, 500000, 1000000};
    static const u32 nnIterations = 10000;

    srand(seed);

    array<i64, size> constructionKDTree = {}, constructionQuadtree = {},
        nnKDTree = {}, nnQuadtree = {};

    for (u32 i = 0; i < numPoints.size(); ++i) {
        u32 n = numPoints[i];

        vector<OsmNode> v;
        v.reserve(n);
        for (u32 _ = 0; _ < n; ++_) {
            v.push_back({Coordinates(randomDouble(minC, maxC), randomDouble(minC, maxC))});
        }

        vector<reference_wrapper<OsmNode>> refV;
        refV.reserve(n);
        for (OsmNode& n : v) {
            refV.push_back(n);
        }
        
        auto start = high_resolution_clock::now();
        KDTree kdt(refV);
        auto end = high_resolution_clock::now();
        constructionKDTree[i] = interval<microseconds>(start, end);

        start = high_resolution_clock::now();
        Quadtree qt(AABB(Coordinates(minC, minC), Coordinates(maxC, maxC)));
        for (const auto& n : v) {
            qt.insert(n);
        }
        end = high_resolution_clock::now();
        constructionQuadtree[i] = interval<microseconds>(start, end);

        for (u32 _ = 0; _ < nnIterations; ++_) {
            Coordinates r(randomDouble(minC, maxC), randomDouble(minC, maxC));

            const OsmNode* kdr, * qtr;

            auto start = high_resolution_clock::now();
            kdr = kdt.nearestNeighbor(r);
            auto end = high_resolution_clock::now();
            auto us = interval<nanoseconds>(start, end);
            nnKDTree[i] += us;

            start = high_resolution_clock::now();
            qtr = qt.nearestNeighbor(r);
            end = high_resolution_clock::now();
            us = interval<nanoseconds>(start, end);
            nnQuadtree[i] += us;
        }
        nnKDTree[i] /= nnIterations;
        nnQuadtree[i] /= nnIterations;
    }

    cout << "Construction - O(n log n)\n";
    cout << string(50, '-') << "\n";
    cout << "Points\t|K-d Tree (us)\t|Quadtree (us)\n";

    for (u32 i = 0; i < numPoints.size(); ++i) {
        cout << numPoints[i] << "\t|" << constructionKDTree[i] << "\t\t|"
            << constructionQuadtree[i] << "\n";
    }

    cout << "\nNearest Neighbor - O(log n) average, O(n) worst case [averaged over "
        << nnIterations << " iterations]\n";
    cout << string(50, '-') << "\n";
    cout << "Points\t|K-d Tree (ns)\t|Quadtree (ns)\n";

    for (u32 i = 0; i < numPoints.size(); ++i) {
        cout << numPoints[i] << "\t|" << nnKDTree[i] << "\t\t|"
            << nnQuadtree[i] << "\n";
    }
}

void quadtreeRealDataComplexityAnalysis(u32 seed) {
    static const u32 nnIterations = 10000;
    i64 nnQuadtree = 0;

    // Get nodes
    OsmXmlData data = parseOsmXml("../data/pa.xml");
    AABB boundary(data.minCoords, data.maxCoords);
    Quadtree quadtree(boundary);

    // Insert nodes into Quadtree
    for (pair<u64, OsmNode> node : data.graph.getNodes()) {
        quadtree.insert(data.graph.getNode(node.first));
    }

    srand(seed);

    // Nearest Neighbor Iterations
    for (u32 _ = 0; _ < nnIterations; ++_) {
        Coordinates point(
            randomDouble(data.minCoords.getLatitude (), data.maxCoords.getLatitude ()),
            randomDouble(data.minCoords.getLongitude(), data.maxCoords.getLongitude())
        );

        auto start = high_resolution_clock::now();
        quadtree.nearestNeighbor(point);
        auto end = high_resolution_clock::now();
        auto us = interval<nanoseconds>(start, end);
        nnQuadtree += us;
    }
    nnQuadtree /= nnIterations;
    cout << nnQuadtree << endl;
}

void fibonacciHeapComplexityAnalysis(u32 seed) {
    static const size_t size = 9;
    static const double minKey = 0, maxKey = 500;
    static const array<u32, size> numNodes = {1000, 5000, 10000,
        50000, 100000, 500000, 1000000, 5000000, 10000000};
    static const u32 insertIters = 100, extractMinIters = 100,
        decreaseKeyIters = 250;

    srand(seed);

    array<u64, size> insert = {}, extractMin = {}, decreaseKey = {};

    u32 current = 0;
    {
        FibonacciHeap<bool> heap;
        for (u32 i = 0; i <= *numNodes.rbegin(); ++i) {
            auto start = high_resolution_clock::now();
            heap.insert(true, randomDouble(minKey, maxKey));
            auto end = high_resolution_clock::now();

            if (i < numNodes[current]) {
                if (i >= numNodes[current] - insertIters) {
                    insert[current] += interval<nanoseconds>(start, end);
                }
            }
            else {
                insert[current] /= insertIters;
                ++current;
            }
        }
    }

    current = 0;
    {
        FibonacciHeap<bool> heap;
        for (u32 i = 0; i <= *numNodes.rbegin(); ++i) {
            heap.insert(true, randomDouble(minKey, maxKey));

            if (i == numNodes[current]) {
                for (u32 _ = 0; _ < extractMinIters; ++_) {
                    auto start = high_resolution_clock::now();
                    heap.extractMin();
                    auto end = high_resolution_clock::now();
                    extractMin[current] += interval<microseconds>(start, end);
                }
                extractMin[current] /= extractMinIters;

                for (u32 _ = 0; _ < extractMinIters; ++_) {
                    heap.insert(true, randomDouble(minKey, maxKey));
                }
                ++current;
            }
        }
    }

    current = 0;
    {
        FibonacciHeap<bool> heap;
        vector<FHNode<bool>*> v;
        v.reserve(*numNodes.rbegin() + 1);

        for (u32 i = 0; i <= *numNodes.rbegin(); ++i) {
            v.push_back(heap.insert(true, randomDouble(minKey, maxKey)));

            if (i == numNodes[current]) {
                for (u32 _ = 0; _ < decreaseKeyIters; ++_) {
                    FHNode<bool>* node = v[rand() % v.size()];
                    auto start = high_resolution_clock::now();
                    heap.decreaseKey(node, randomDouble(minKey, node->key));
                    auto end = high_resolution_clock::now();
                    decreaseKey[current] += interval<nanoseconds>(start, end);
                }
                decreaseKey[current] /= decreaseKeyIters;
                ++current;
            }
        }
    }

    cout << "Insertion - O(1)\n";
    cout << string(50, '-') << "\n";
    cout << setw(10) << "Nodes" << " | Time (ns)\n";

    for (u32 i = 0; i < numNodes.size(); ++i) {
        cout << setw(10) << numNodes[i] << " | " << insert[i] << "\n";
    }

    cout << "\nExtract Min - O(log n)\n";
    cout << string(50, '-') << "\n";
    cout << setw(10) << "Nodes" << " | Time (us)\n";

    for (u32 i = 0; i < numNodes.size(); ++i) {
        cout << setw(10) << numNodes[i] << " | " << extractMin[i] << "\n";
    }

    cout << "\nDecrease Key - O(1)\n";
    cout << string(50, '-') << "\n";
    cout << setw(10) << "Nodes" << " | Time (ns)\n";

    for (u32 i = 0; i < numNodes.size(); ++i) {
        cout << setw(10) << numNodes[i] << " | " << decreaseKey[i] << "\n";
    }
}
