
#ifndef A_STAR_H
#define A_STAR_H

#include <utility>
#include <list>
#include <map>
#include "../types.hpp"
#include "../graph.hpp"
#include "../data_structures/fibonacci_heap.hpp"

template <typename T>
std::pair<std::list<u64>, double> aStarSearch(Graph<T> g, u64 start, u64 end) {
    FibonacciHeap<u64> heap;
    std::map<u64, u64> predecessorMap;
    std::map<u64, double> currentCostMap;

    heap.insert(start, 0);

    while (!heap.empty()) {
        u64 min = heap.extractMin();
    }

    // Failed to find a path between start and end
    return std::make_pair({}, 0);
}

#endif // A_STAR_H
