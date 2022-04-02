
#include <array>
#include <chrono>
#include <iostream>
#include <random>
#include "complexity.hpp"
#include "../types.hpp"
#include "../data_structures/kd_tree.hpp"
#include "../data_structures/quadtree.hpp"

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

void kdTreeQuadtreeComplexityAnalysis(unsigned int seed) {
    srand(seed);

    const array<u32, 10> numPoints = {100, 500, 1000, 2500, 5000, 10000,
        50000, 100000, 500000, 1000000};
    const double minC = -75, maxC = 75;
    const u32 nnIterations = 10000;
    array<i64, 10> constructionKDTree = {}, constructionQuadtree = {},
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
