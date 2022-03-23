#include <iostream>
#include "fibonacci_heap.hpp"

using namespace std;

template <typename T>
void FibonacciHeap<T>::insert(T data, double key) {
    FHNode<T>* newNode = new FHNode<T>(data, key);
    if ((min == nullptr) || (lastNodeOnRootList == nullptr)) {
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

template <typename T>
void FibonacciHeap<T>::show() {
    cout << "Size: " << size << endl;
    FHNode<T>* node = min;
    do {
        cout << node->key << "\t";
        node = node->next;
    } while (node != min);
    cout << endl;
}
