
#ifndef BINARY_HEAP_H
#define BINARY_HEAP_H

#include <vector>
#include <optional>
#include <unordered_map>

template <typename T>
struct BHNode {
    BHNode(T data, double key) : data(data), key(key) {}

    T data;
    double key;
};

template <typename T>
class BinaryHeap {
    public:
        ~BinaryHeap() {
            for (const BHNode<T>* n : vec) {
                delete n;
            }
        }

        BHNode<T>* insert(T data, double key) {
            size_t index = vec.size();

            vec.push_back(new BHNode {data, key});
            BHNode<T>* ret = vec.back();
            indices[ret] = index;

            heapifyUp(index);
            return ret;
        }

        std::optional<T> extractMin() {
            if (vec.empty()) {
                return std::nullopt;
            }

            BHNode<T>* ptr = vec.front();
            T root = ptr->data;
            if (vec.size() != 1) {
                swap(0, vec.size() - 1);
                vec.pop_back();
                heapifyDown(0);
            }
            else {
                vec.pop_back();
            }
            indices.erase(ptr);
            delete ptr;

            return root;
        }

        void decreaseKey(BHNode<T>* node, double key) {
            if (node) {
                if (key < node->key) {
                    node->key = key;
                    heapifyUp(indices[node]);
                }
            }
        }

        bool empty() const {
            return vec.empty();
        }

        friend std::ostream& operator<<(std::ostream& os, const BinaryHeap<T>& obj) {
            for (const BHNode<T>* n : obj.vec) {
                os << n->data << "[" << n->key << "]" << " ";
            }
            return os;
        }
    private:
        void swap(size_t a, size_t b) {
            auto temp = vec[a];
            vec[a] = vec[b];
            vec[b] = temp;
            indices[temp] = b;
            indices[vec[a]] = a;
        }

        void heapifyUp(size_t index) {
            while (index > 0 && vec[index]->key < vec[parent(index)]->key) {
                swap(index, parent(index));
                index = parent(index);
            }
        }

        void heapifyDown(size_t index) {
            size_t left = leftChild(index), right = rightChild(index), size = vec.size();
            size_t min = index;

            if (left < size) {
                if (vec[left]->key < vec[min]->key) {
                    min = left;
                }
            }

            if (right < size) {
                if (vec[right]->key < vec[min]->key) {
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

        std::vector<BHNode<T>*> vec;
        std::unordered_map<BHNode<T>*, size_t> indices;
};

#endif // BINARY_HEAP_H
