
#ifndef FIBONACCI_HEAP_H
#define FIBONACCI_HEAP_H

template <typename T>
struct FHNode {
    FHNode(T data, double key) : data(data), key(key) {}

    FHNode<T>* prev = nullptr, * next = nullptr, * parent = nullptr,
        * child = nullptr;
    int degree = 0;
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

        void insert(T data, double key) {
            FHNode<T>* n = new FHNode<T>(data, key);
            n->next = n;
            n->prev = n;

            FibonacciHeap<T> heap;
            heap.min = n;
            heap.size = 1;

            merge(heap);
            return;
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

        friend std::ostream& operator<<(std::ostream& os, const FibonacciHeap<T>& obj) {
            os << "Size: " << obj.size << '\n';
            const FHNode<T>* node = obj.min;
            do {
                os << node->key << '\t';
                node = node->next;
            } while (node != obj.min);

            return os;
        }
    private:
        void deleteAll(FHNode<T>* node) {
            if (node) {
                FHNode<T>* next = node, * prev;
                do {
                    prev = next;
                    next = next->next;
                    deleteAll(prev->child);
                    delete prev;
                } while (next != node);
            }
        }

        FHNode<T>* min = nullptr;
        int size = 0;
};

#endif // FIBONACCI_HEAP_H
