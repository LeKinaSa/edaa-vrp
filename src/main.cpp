#include <iostream>
#include "algorithms/a_star.hpp"
#include "data_structures/fibonacci_heap.hpp"
#include "coordinates.hpp"
#include "graph.hpp"

using namespace std;

Graph<OsmNode> getGraph() {
    Graph<OsmNode> g;

    Coordinates c0(0, 0), c1(4, 0), c2(8.5, 3.8), c3(7.3, 7.8),
            c4(1, 2.8), c5(3.8, 6), c6(0, 10.8);

    OsmNode n0 = {c0}, n1 = {c1}, n2 = {c2}, n3 = {c3}, n4 = {c4},
            n5 = {c5}, n6 = {c6};

    g.addNode(0, n0);
    g.addNode(1, n1);
    g.addNode(2, n2);
    g.addNode(3, n3);
    g.addNode(4, n4);
    g.addNode(5, n5);
    g.addNode(6, n6);

    g.addEdge(0, 1, c0.haversine(c1));
    g.addEdge(1, 2, c1.haversine(c2));
    g.addEdge(2, 3, c2.haversine(c3));
    g.addEdge(0, 4, c0.haversine(c4));
    g.addEdge(4, 5, c4.haversine(c5));
    g.addEdge(5, 6, c5.haversine(c6));
    g.addEdge(6, 3, c6.haversine(c3));

    return g;
}

void aStarTest() {
    // Obtain the Graph to be used in the A* Search
    Graph<OsmNode> g = getGraph();

    // Deciding the Start Node
    u64 start = 0;
    // Deciding the End Node
    u64 end = 3;

    // Search
    pair<list<u64>, double> result;

    // A* Search
    cout << endl << "A* Search" << endl;
    result = aStarSearch(g, start, end);
    
    for (auto element : result.first) {
        cout << element << "\t";
    }
    cout << endl << result.second << endl;

    // Iterative Deepening A* Search
    cout << endl << "Iterative Deepening A* Search" << endl;
    result = iterativeDeepeningAStarSearch(g, start, end);
    
    for (auto element : result.first) {
        cout << element << "\t";
    }
    cout << endl << result.second << endl;

    // Simplified Memory Bound A* Search
    cout << endl << "Simplified Memory Bound A* Search" << endl;
    result = simpleMemoryBoundedAStarSearch(g, start, end, 2);
    
    for (auto element : result.first) {
        cout << element << "\t";
    }
    cout << endl << result.second << endl;
}

int main() {
    aStarTest();
    return 0;
}
