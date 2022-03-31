
#include <array>
#include <chrono>
#include <iostream>
#include <random>
#include "complexity.hpp"
#include "../types.hpp"
#include "../data_structures/kd_tree.hpp"

using namespace std;
using chrono::high_resolution_clock;
using chrono::duration_cast;
using chrono::microseconds;

double randomDouble(double min, double max) {
    return min + static_cast<double>(rand()) / static_cast<double>(RAND_MAX / (max - min));
}

void kdTreeComplexityAnalysis(unsigned int seed) {
    srand(seed);

    const array<u32, 10> numPoints = {100, 500, 1000, 2500, 5000, 10000,
        50000, 100000, 500000, 1000000};
    const double minC = -75, maxC = 75;
    const u32 nnIterations = 100;
    array<i64, 10> nnResults = {};

    cout << "Construction - O(n log n)\n";

    for (u32 i = 0; i < numPoints.size(); ++i) {
        u32 n = numPoints[i];

        vector<OsmNode> v;
        v.reserve(n);
        for (u32 _ = 0; _ < n; ++_) {
            v.push_back({Coordinates(randomDouble(minC, maxC), randomDouble(minC, maxC))});
        }

        vector<reference_wrapper<OsmNode>> refV;
        refV.reserve(n);
        for (auto n : v) {
            refV.push_back(ref(n));
        }
        
        auto start = high_resolution_clock::now();
        KDTree t(refV);
        auto end = high_resolution_clock::now();

        for (u32 _ = 0; _ < nnIterations; ++_) {
            Coordinates r(randomDouble(minC, maxC), randomDouble(minC, maxC));

            auto start = high_resolution_clock::now();
            t.nearestNeighbor(r);
            auto end = high_resolution_clock::now();
            auto us = duration_cast<microseconds>(end - start).count();
            nnResults[i] += us;
        }
        nnResults[i] /= nnIterations;

        cout << "\t- " << n << " points finished in "
            << chrono::duration_cast<microseconds>(end - start).count() << "us\n";
    }

    cout << "\nNearest Neighbor - O(log n) average, O(n) worst case [averaged over "
        << nnIterations << " iterations]\n";
    for (u32 i = 0; i < numPoints.size(); ++i) {
        cout << "\t- " << numPoints[i] << " points finished in average of " << nnResults[i] << "us\n";
    }
}
