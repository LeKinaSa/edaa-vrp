
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
    private:
        FHNode<T>* min = nullptr;
        int size = 0;
};

#endif // FIBONACCI_HEAP_H
