
#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <unordered_map>

template <typename T>
class Vertex {
    public:
        const T& getData() const;
        const std::unordered_map<Vertex<T>*, double> getAdjacent() const;
    private:
        T data;
        std::unordered_map<Vertex<T>*, double> adjacent;
};

template <typename T>
class Graph {
    public:
        const std::vector<Vertex<T>>& getVertices() const;
    private:
        std::vector<Vertex<T>> vertices;
};

#endif // GRAPH_H
