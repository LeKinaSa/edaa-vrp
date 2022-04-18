
#ifndef BINARY_HEAP_H
#define BINARY_HEAP_H

#include <vector>
#include <optional>
#include <unordered_map>
#include "../types.hpp"

template <typename T>
struct BHNode {
    BHNode(u64 id, T data, double key) : id(id), data(data), key(key) {}

    u64 id;
    T data;
    double key;
};

template <typename T>
class BinaryHeap {
    public:
        explicit BinaryHeap(size_t reserveSize = 0) {
            vec.reserve(reserveSize);
        }

        u64 insert(T data, double key) {
            size_t index = vec.size();
            u64 id = nextId;

            vec.push_back({id, data, key});
            indices[id] = index;

            heapifyUp(index);
            return id;
        }

        T extractMin() {
            BHNode<T>& min = vec.front();
            T root = min.data;

            if (vec.size() != 1) {
                swap(0, vec.size() - 1);
                vec.pop_back();
                heapifyDown(0);
            }
            else {
                vec.pop_back();
            }
            indices.erase(min.id);

            return root;
        }

        void decreaseKey(u64 id, double key) {
            if (indices.count(id)) {
                BHNode<T>& node = vec[indices[id]];
                if (key < node.key) {
                    node.key = key;
                    heapifyUp(indices[id]);
                }
            }
        }

        bool empty() const {
            return vec.empty();
        }

        friend std::ostream& operator<<(std::ostream& os, const BinaryHeap<T>& obj) {
            for (const BHNode<T>& n : obj.vec) {
                os << n.data << "[" << n.key << "]" << " ";
            }
            return os;
        }
    private:
        void swap(size_t a, size_t b) {
            BHNode<T> temp = vec[a];
            vec[a] = vec[b];
            vec[b] = temp;
            indices[vec[a].id] = a;
            indices[vec[b].id] = b;
        }

        void heapifyUp(size_t index) {
            while (index > 0 && vec[index].key < vec[parent(index)].key) {
                swap(index, parent(index));
                index = parent(index);
            }
        }

        void heapifyDown(size_t index) {
            size_t left = leftChild(index), right = rightChild(index), size = vec.size();
            size_t min = index;

            if (left < size) {
                if (vec[left].key < vec[min].key) {
                    min = left;
                }
            }

            if (right < size) {
                if (vec[right].key < vec[min].key) {
                    min = right;
                }
            }

            if (min != index) {
                swap(index, min);
                heapifyDown(min);
            }
        }

        size_t parent(size_t index) {
            return (index - 1) / 2;
        }
        size_t leftChild(size_t index) {
            return 2 * index + 1;
        }
        size_t rightChild(size_t index) {
            return 2 * index + 2;
        }

        u64 nextId = 0;
        std::vector<BHNode<T>> vec;
        std::unordered_map<u64, size_t> indices;
};

#endif // BINARY_HEAP_H
