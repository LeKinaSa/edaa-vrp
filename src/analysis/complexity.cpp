
#include <array>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <random>
#include "complexity.hpp"
#include "../data_structures/kd_tree.hpp"
#include "../data_structures/quadtree.hpp"
#include "../data_structures/binary_heap.hpp"
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
            v.push_back({0, Coordinates(randomDouble(minC, maxC), randomDouble(minC, maxC))});
        }

        vector<reference_wrapper<const OsmNode>> refV;
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
    for (const pair<u64, OsmNode>& node : data.graph.getNodes()) {
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

void heapComplexityAnalysis(u32 seed) {
    static const size_t size = 7;
    static const double minKey = 0, maxKey = 500;
    static const array<u32, size> numNodes = {1000, 5000, 10000,
        50000, 100000, 500000, 1000000};
    static const u32 insertIters = 100, extractMinIters = 100,
        decreaseKeyIters = 250;

    srand(seed);
    random_device rd;
    default_random_engine eng(rd());
    uniform_real_distribution<double> randDist(minKey, maxKey);

    array<u64, size> insertBin = {}, extractMinBin = {}, decreaseKeyBin = {},
        insertFib = {}, extractMinFib = {}, decreaseKeyFib = {};

    u32 current = 0;
    {
        BinaryHeap<bool> binHeap;
        FibonacciHeap<bool> fibHeap;
        for (u32 i = 0; i <= *numNodes.rbegin(); ++i) {
            double key = randDist(eng);
            
            auto startBin = high_resolution_clock::now();
            binHeap.insert(true, key);
            auto endBin = high_resolution_clock::now();

            auto startFib = high_resolution_clock::now();
            fibHeap.insert(true, key);
            auto endFib = high_resolution_clock::now();

            if (i < numNodes[current]) {
                if (i >= numNodes[current] - insertIters) {
                    insertBin[current] += interval<nanoseconds>(startBin, endBin);
                    insertFib[current] += interval<nanoseconds>(startFib, endFib);
                }
            }
            else {
                insertBin[current] /= insertIters;
                insertFib[current] /= insertIters;
                ++current;
            }
        }
    }

    current = 0;
    {
        BinaryHeap<bool> binHeap;
        FibonacciHeap<bool> fibHeap;
        for (u32 i = 0; i <= *numNodes.rbegin(); ++i) {
            double key = randDist(eng);
            binHeap.insert(true, key);
            fibHeap.insert(true, key);

            if (i == numNodes[current]) {
                for (u32 _ = 0; _ < extractMinIters; ++_) {
                    auto start = high_resolution_clock::now();
                    binHeap.extractMin();
                    auto end = high_resolution_clock::now();
                    extractMinBin[current] += interval<microseconds>(start, end);

                    start = high_resolution_clock::now();
                    fibHeap.extractMin();
                    end = high_resolution_clock::now();
                    extractMinFib[current] += interval<microseconds>(start, end);
                }
                extractMinBin[current] /= extractMinIters;
                extractMinFib[current] /= extractMinIters;

                for (u32 _ = 0; _ < extractMinIters; ++_) {
                    double key = randomDouble(minKey, maxKey);
                    binHeap.insert(true, key);
                    fibHeap.insert(true, key);
                }
                ++current;
            }
        }
    }

    current = 0;
    {
        BinaryHeap<bool> binHeap;
        vector<BHNode<bool>*> vBin;
        vBin.reserve(*numNodes.rbegin());
        
        FibonacciHeap<bool> fibHeap;
        vector<FHNode<bool>*> vFib;
        vFib.reserve(*numNodes.rbegin());

        for (u32 i = 0; i < *numNodes.rbegin(); ++i) {
            double key = randDist(eng);
            vBin.push_back(binHeap.insert(true, key));
            vFib.push_back(fibHeap.insert(true, key));

            if (i == numNodes[current] - 1) {
                for (u32 _ = 0; _ < 5; ++_) {
                    fibHeap.insert(true, minKey - 10);
                    fibHeap.extractMin();
                }

                for (u32 _ = 0; _ < decreaseKeyIters; ++_) {
                    size_t idx = rand() % vBin.size();
                    uniform_real_distribution<double> tempDist(minKey, vBin[idx]->key);
                    double newKey = tempDist(eng);

                    auto start = high_resolution_clock::now();
                    binHeap.decreaseKey(vBin[idx], newKey);
                    auto end = high_resolution_clock::now();
                    decreaseKeyBin[current] += interval<nanoseconds>(start, end);

                    start = high_resolution_clock::now();
                    fibHeap.decreaseKey(vFib[idx], newKey);
                    end = high_resolution_clock::now();
                    decreaseKeyFib[current] += interval<nanoseconds>(start, end);
                }
                decreaseKeyBin[current] /= decreaseKeyIters;
                decreaseKeyFib[current] /= decreaseKeyIters;
                ++current;
            }
        }
    }

    cout << "Insertion - O(1)\n";
    cout << string(50, '-') << "\n";
    cout << setw(10) << "Nodes" << " | " << setw(10) << "Bin (ns)" << " | "
        << setw(10) << "Fib (ns)" << "\n";

    for (u32 i = 0; i < numNodes.size(); ++i) {
        cout << setw(10) << numNodes[i] << " | " << setw(10) << insertBin[i] 
            << " | " << setw(10) << insertFib[i] << "\n";
    }

    cout << "\nExtract Min - O(log n)\n";
    cout << string(50, '-') << "\n";
    cout << setw(10) << "Nodes" << " | " << setw(10) << "Bin (ns)" << " | "
        << setw(10) << "Fib (ns)" << "\n";

    for (u32 i = 0; i < numNodes.size(); ++i) {
        cout << setw(10) << numNodes[i] << " | " << setw(10) << extractMinBin[i] 
            << " | " << setw(10) << extractMinFib[i] << "\n";
    }

    cout << "\nDecrease Key - O(log n) [Bin], O(1) [Fib]\n";
    cout << string(50, '-') << "\n";
    cout << setw(10) << "Nodes" << " | " << setw(10) << "Bin (ns)" << " | "
        << setw(10) << "Fib (ns)" << "\n";

    for (u32 i = 0; i < numNodes.size(); ++i) {
        cout << setw(10) << numNodes[i] << " | " << setw(10) << decreaseKeyBin[i] 
            << " | " << setw(10) << decreaseKeyFib[i] << "\n";
    }
}
