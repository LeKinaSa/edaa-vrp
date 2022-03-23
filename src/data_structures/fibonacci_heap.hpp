
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
        void insert(T data, double key);

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
        FHNode<T>* min = nullptr;
        FHNode<T>* lastNodeOnRootList = nullptr;
        int size = 0;
};

#endif // FIBONACCI_HEAP_H
