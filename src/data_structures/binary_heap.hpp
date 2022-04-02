
#ifndef BINARY_HEAP_H
#define BINARY_HEAP_H

#include <vector>
#include <optional>
#include <unordered_map>

template <typename T>
class BinaryHeap {
    public:
        std::pair<T, double>& insert(T data, double key) {
            size_t index = vec.size();

            vec.push_back(std::make_pair(data, key));
            auto ret = vec.back();
            indices[&ret] = index;

            heapifyUp(index);
            return ret;
        }

        std::optional<T> extractMin() {
            if (vec.empty()) {
                return std::nullopt;
            }

            std::pair<T, double>* ptr = &vec.front();
            T root = vec.front();
            if (vec.size() != 1) {
                swap(0, vec.size() - 1);
                vec.pop_back();
                heapifyDown(0);
            }
            else {
                vec.pop_back();
            }
            indices.erase(ptr);

            return root;
        }

        void decreaseKey(std::pair<T, double>& node, double key) {
            if (key < node.second) {
                node.second = key;
                heapifyUp(indices[&node]);
            }
        }
    private:
        void swap(size_t a, size_t b) {
            std::swap(vec[a], vec[b]);
            indices[&vec[a]] = b;
            indices[&vec[b]] = a;
        }

        void heapifyUp(size_t index) {
            while (index > 0 && vec[index] < vec[parent(index)]) {
                swap(index, parent(index));
                index = parent(index);
            }
        }

        void heapifyDown(size_t index) {
            size_t left = leftChild(index), right = rightChild(index), size = vec.size();
            size_t min = index;

            if (left < size) {
                if (vec[left] < vec[min]) {
                    min = left;
                }
            }

            if (right < size) {
                if (vec[right] < vec[min]) {
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

        std::vector<std::pair<T, double>> vec;
        std::unordered_map<std::pair<T, double>*, size_t> indices;
};

#endif // BINARY_HEAP_H
