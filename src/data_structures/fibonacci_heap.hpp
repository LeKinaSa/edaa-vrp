
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
            FHNode<T>* newNode = new FHNode<T>(data, key);
            if (min == nullptr || lastNodeOnRootList == nullptr) {
                min = newNode;
                lastNodeOnRootList = newNode;
                newNode->prev = newNode;
                newNode->next = newNode;
            }
            else {
                newNode->prev = lastNodeOnRootList;
                newNode->next = min;
                min->prev = newNode;
                lastNodeOnRootList->next = newNode;

                if (newNode->key < min->key) {
                    min = newNode;
                }
                else {
                    lastNodeOnRootList = newNode;
                }
            }
            ++ size;
        }

        friend std::ostream& operator<<(std::ostream& os, const FibonacciHeap<T>& obj) {
            os << "Size: " << obj.size << std::endl;
            FHNode<T>* node = obj.min;
            do {
                os << node->key << "\t";
                node = node->next;
            } while (node != obj.min);
            
            return os;
        }
    private:
        void deleteAll(FHNode<T>* node) {
            if (node) {
                FHNode<T>* next = node, prev;
                do {
                    prev = next;
                    next = next->next;
                    deleteAll(prev->child);
                    delete d;
                } while (next != node);
            }
        }

        FHNode<T>* min = nullptr;
        FHNode<T>* lastNodeOnRootList = nullptr;
        int size = 0;
};

#endif // FIBONACCI_HEAP_H
