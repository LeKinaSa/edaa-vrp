#include <iostream>
#include "algorithms/a_star.hpp"
#include "data_structures/fibonacci_heap.hpp"
#include "coordinates.hpp"
#include "graph.hpp"

using namespace std;

void aStarTest() {
    // Obtain the Graph to be used in the A* Search
    Graph<OsmNode>* g = new Graph<OsmNode>();

    Coordinates* c0 = new Coordinates(0  , 0  );
    Coordinates* c1 = new Coordinates(4  , 0  );
    Coordinates* c2 = new Coordinates(8.5, 3.8);
    Coordinates* c3 = new Coordinates(7.3, 7.8);
    Coordinates* c4 = new Coordinates(1  , 2.8);
    Coordinates* c5 = new Coordinates(3.8, 6  );
    Coordinates* c6 = new Coordinates(0  ,10.8);

    OsmNode n0; n0.coordinates = *c0;
    OsmNode n1; n1.coordinates = *c1;
    OsmNode n2; n2.coordinates = *c2;
    OsmNode n3; n3.coordinates = *c3;
    OsmNode n4; n4.coordinates = *c4;
    OsmNode n5; n5.coordinates = *c5;
    OsmNode n6; n6.coordinates = *c6;

    g->addNode(0, n0);
    g->addNode(1, n1);
    g->addNode(2, n2);
    g->addNode(3, n3);
    g->addNode(4, n4);
    g->addNode(5, n5);
    g->addNode(6, n6);

    g->addEdge(0, 1, 4);
    g->addEdge(1, 2, 6);
    g->addEdge(2, 3, 4);
    g->addEdge(0, 4, 3);
    g->addEdge(4, 5, 4);
    g->addEdge(5, 6, 6);
    g->addEdge(6, 3, 8);

    // Deciding the Start Node
    u64 start = 0;
    // Deciding the End Node
    u64 end = 3;
    // A* Search
    pair<list<u64>, double> result = aStarSearch(*g, start, end);
    cout << result.second << endl;
}

int main() {
    aStarTest();
    return 0;
}
