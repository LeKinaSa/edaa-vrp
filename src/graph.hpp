
#ifndef GRAPH_H
#define GRAPH_H

#include <list>
#include <unordered_map>
#include <string>
#include "types.hpp"

template <typename T>
class Graph {
    public:
        Graph() : adjList({}) {}
        void addNode(u64 id, T data) {
            nodes[id] = data;
            adjList[id] = {};
        }

        void addEdge(u64 node1, u64 node2, double weight) {
            if (adjList.count(node1) != 0 && adjList.count(node2) != 0) {
                adjList[node1].push_back(std::make_pair(node2, weight));
            }
        }

        const T& getNode(u64 id) const {
            return nodes.at(id);
        }

        T& getNode(u64 id) {
            return nodes.at(id);
        }

        const std::list<std::pair<u64, double>>& getEdges(u64 id) const {
            return adjList.at(id);
        }

        const std::unordered_map<u64, T>& getNodes() const {
            return nodes;
        }

        std::unordered_map<u64, T>& getNodes() {
            return nodes;
        }

        size_t numEdges() const {
            size_t n = 0;
            for (const auto& p : adjList) {
                n += p.second.size();
            }
            return n;
        }
    private:
        std::unordered_map<u64, T> nodes;
        std::unordered_map<u64, std::list<std::pair<u64, double>>> adjList;
};

#endif // GRAPH_H
