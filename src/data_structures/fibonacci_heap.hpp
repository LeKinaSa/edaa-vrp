
#ifndef FIBONACCI_HEAP_H
#define FIBONACCI_HEAP_H

#include <cmath>
#include <list>
#include <vector>

#include "../types.hpp"

template <typename T>
struct FHNode {
    FHNode(T data, double key) : data(data), key(key) {}

    FHNode<T>* prev = nullptr, * next = nullptr, * parent = nullptr,
        * child = nullptr;
    u32 degree = 0;
    bool marked = false;
    T data;
    double key;
};

template <typename T>
class FibonacciHeap {
    public:
        ~FibonacciHeap() {
            if (min) {
                deleteAll(min);
            }
        }

        bool empty() const {
            return size == 0;
        }

        u32 getSize() const {
            return size;
        }

        FHNode<T>* insert(T data, double key) {
            FHNode<T>* n = new FHNode<T>(data, key);
            n->next = n;
            n->prev = n;

            FibonacciHeap<T> heap;
            heap.min = n;
            heap.size = 1;

            merge(heap);
            return n;
        }

        void merge(FibonacciHeap& other) {
            if (other.min != nullptr) {
                if (min != nullptr) {
                    FHNode<T>* minNext = min->next;
                    min->next = other.min->next;
                    min->next->prev = min;
                    other.min->next = minNext;
                    other.min->next->prev = other.min;

                    if (other.min->key < min->key) {
                        min = other.min;
                    }
                }
                else {
                    min = other.min;
                }

                size += other.size;
                other.min = nullptr;
                other.size = 0;
            }
        }

        T extractMin() {
            FHNode<T>* ret = min;

            if (ret->child) {
                // Add children to root list
                for (FHNode<T>* child : iterate(ret->child)) {
                    child->prev = min;
                    child->next = min->next;
                    min->next->prev = child;
                    min->next = child;

                    child->parent = nullptr;
                }
            }

            removeFromRootList(ret);

            if (ret->next == ret) {
                // Heap had a single node
                min = nullptr;
            }
            else {
                min = ret->next;
                consolidate();
            }

            // Extract data and free memory
            size -= 1;
            T data = ret->data;
            delete ret;
            return data;
        }

        T extractMax() {
            // Find max
            FHNode<T>* max = getMax(min);

            // Change the key to -1
            decreaseKey(max, -1);

            // Extract min
            return extractMin();
        }

        void decreaseKey(FHNode<T>* node, double key) {
            if (key >= node->key) {
                return;
            }

            node->key = key;

            FHNode<T>* parent = node->parent;
            if (parent) {
                if (node->key < parent->key) {
                    cut(node);
                    cascadingCut(parent);
                }
            }
            
            if (node->key < min->key) {
                min = node;
            }
        }

        friend std::ostream& operator<<(std::ostream& os, const FibonacciHeap<T>& obj) {
            os << "Size: " << obj.size << '\n';
            os << std::string(20, '-') << '\n';
            obj.printNodes(os, obj.min);
            return os;
        }
    private:
        void printNodes(std::ostream& os, const FHNode<T>* head, u32 depth = 0) const {
            for (const FHNode<T>* node : iterate(head)) {
                os << std::string(depth, '>') << node->key << "\t\t\t" << node->data;
                if (node->marked) os << '*';
                os << '\n';
                if (node->child) {
                    printNodes(os, node->child, depth + 1);
                }
            }
        }

        void deleteAll(FHNode<T>* head) {
            for (FHNode<T>* node : iterate(head)) {
                if (node->child) {
                    deleteAll(node->child);
                }
                delete node;
            }
        }

        std::list<FHNode<T>*> iterate(FHNode<T>* head) {
            std::list<FHNode<T>*> nodes;

            if (head) {
                const FHNode<T>* stop = head;

                do {
                    nodes.push_back(head);
                    head = head->next;
                } while (head != stop);
            }

            return nodes;
        }

        std::list<const FHNode<T>*> iterate(const FHNode<T>* head) const {
            std::list<const FHNode<T>*> nodes;

            if (head) {
                const FHNode<T>* stop = head;

                do {
                    nodes.push_back(head);
                    head = head->next;
                } while (head != stop);
            }

            return nodes;
        }

        void removeFromRootList(FHNode<T>* node) {
            if (node) {
                node->prev->next = node->next;
                node->next->prev = node->prev;
            }
        }

        void addChild(FHNode<T>* node, FHNode<T>* newChild) {
            if (node && newChild) {
                if (node->child) {
                    newChild->prev = node->child;
                    newChild->next = node->child->next;
                    node->child->next->prev = newChild;
                    node->child->next = newChild;
                }
                else {
                    newChild->prev = newChild;
                    newChild->next = newChild;
                    node->child = newChild;
                }

                node->degree += 1;
                newChild->parent = node;
                newChild->marked = false;
            }
        }

        void removeChild(FHNode<T>* child) {
            FHNode<T>* parent = child->parent;

            if (parent) {
                if (parent->child == child) {
                    if (parent->child == parent->child->next) {
                        // Parent only has one child
                        parent->child = nullptr;
                    }
                    else {
                        parent->child = child->next;
                    }
                }
                child->prev->next = child->next;
                child->next->prev = child->prev;
            }
        }

        void consolidate() {
            std::vector<FHNode<T>*> a(ceil(log2(size)) + 1, nullptr);

            for (FHNode<T>* node : iterate(min)) {
                u32 d = node->degree;
                while (a[d]) {
                    FHNode<T>* lower = node, * higher = a[d];
                    if (node->key > higher->key) {
                        lower = higher;
                        higher = node;
                    }
                    node = lower;

                    removeFromRootList(higher);
                    addChild(lower, higher);

                    a[d] = nullptr;
                    ++d;
                }
                a[d] = node;
            }

            // Find new minimum
            for (FHNode<T>* root : a) {
                if (root) {
                    if (root->key <= min->key) {
                        min = root;
                    }
                }
            }
        }

        void cut(FHNode<T>* node) {
            removeChild(node);
            node->parent->degree -= 1;

            // TODO: Create addToRootList method
            node->prev = min;
            node->next = min->next;
            min->next->prev = node;
            min->next = node;

            node->parent = nullptr;
            node->marked = false;
        }

        void cascadingCut(FHNode<T>* node) {
            FHNode<T>* parent = node->parent;

            if (parent) {
                if (!node->marked) {
                    node->marked = true;
                }
                else {
                    cut(node);
                    cascadingCut(parent);
                }
            }
        }

        FHNode<T>* getMax(FHNode<T>* head) {
            FHNode<T>* max = head;

            for (FHNode<T>* node : iterate(head)) {
                if ((max == nullptr) || ((node != nullptr) && (node->key > max->key))) {
                    max = node;
                }

                if (node->child) {
                    node = getMax(node->child);
                    if ((max == nullptr) || ((node != nullptr) && (node->key > max->key))) {
                        max = node;
                    }
                }
            }

            return max;
        }

        FHNode<T>* min = nullptr;
        u32 size = 0;
};

#endif // FIBONACCI_HEAP_H
