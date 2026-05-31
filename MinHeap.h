#pragma once

#include <iostream>

enum STATE {EMPTY, NOT_EMPTY};
using HeapNode = std::pair<double,int>;

//static MinHeap
class MinHeap {
public:
    MinHeap(HeapNode* array, int size);
    MinHeap(std::initializer_list<HeapNode> list);
    std::pair<STATE,int> delMin();
    HeapNode getMin();
    void decreaseKey(int pos, double newValue);

    double operator[](int pos);


    ~MinHeap();
private:
    void siftDown(int index);
    void siftUp(int index);
    void heapify();
    void swap(int index1, int index2);
    int smallerOfBoth(int index);

    bool was_allocated_byMe;
    int size;
    HeapNode* array;
    int* position;
};
