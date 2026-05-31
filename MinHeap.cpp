#include "MinHeap.h"


#define LEFT_CHILD(index) (((index+1) << 1) - 1)
#define RIGHT_CHILD(index) (LEFT_CHILD(index) + 1)
#define PARENT(index) (((index-1) >> 1))


MinHeap::MinHeap(HeapNode* array, int size) : was_allocated_byMe(false), size(size),
array(array) {
    if (size < 0) {
        std::cerr << "Bad size detected" << std::endl;
        throw std::bad_alloc();
    }
    try {
        position = new int[size];
    } catch (std::bad_alloc& e) {
        std::cerr << "Bad allocation detected" << e.what() << std::endl;
        exit(1);
    }
    heapify();
}

MinHeap::MinHeap(std::initializer_list<HeapNode> list) : was_allocated_byMe(true),
size(list.size()) {
    try {
        array = new HeapNode[size];
        position = new int[size];
    } catch (std::bad_alloc& e) {
        std::cerr << "Bad allocation detected" << e.what() << std::endl;
        exit(1);
    }
    int j = 0;
    for (HeapNode pair : list) {
        array[j] = pair;
        j++;
    }

    heapify();
}


MinHeap::~MinHeap() {
    if (was_allocated_byMe) {
        delete[] array;
    }
    delete[] position;
}

HeapNode MinHeap::getMin() {
    return HeapNode(array[0].first, array[0].second);
}

std::pair<STATE,int> MinHeap::delMin() {
    if (this->size <= 0) {
        return std::pair<STATE,int>(EMPTY,-1);
    }
    this->size--;
    int retVal = array[0].second;
    swap(0, size);
    siftDown(0);
    return std::pair<STATE,int>(NOT_EMPTY,retVal);
}

void MinHeap::swap(int index1, int index2) {
    position[array[index1].second] = index2;
    position[array[index2].second] = index1;

    HeapNode temp;
    temp = array[index1];
    array[index1] = array[index2];
    array[index2] = temp;

}

int MinHeap::smallerOfBoth(int index) {
    if (RIGHT_CHILD(index) >= size) {
        return LEFT_CHILD(index);
    } else {
        return array[LEFT_CHILD(index)].first > array[RIGHT_CHILD(index)].first ?
        RIGHT_CHILD(index) : LEFT_CHILD(index);
    }
}

void MinHeap::siftDown(int index) {
    int currentIndex = index;
    while ( (currentIndex < size) && ( LEFT_CHILD(currentIndex) < size ) ) { //right child < left one..
        int smallerOfBothIndex = smallerOfBoth(currentIndex);

        if (array[smallerOfBothIndex].first < array[currentIndex].first) {
            swap(smallerOfBothIndex, currentIndex);
            currentIndex = smallerOfBothIndex;
            continue;
        }

        break;
    }
}

void MinHeap::siftUp(int index) {
    int currentIndex = index;
    while ( (currentIndex > 0) && (PARENT(currentIndex) >= 0) ) {
        if (array[currentIndex].first < array[PARENT(currentIndex)].first) {
            swap(currentIndex, PARENT(currentIndex));
            currentIndex = PARENT(currentIndex);
        } else {
            break;
        }
    }
}

void MinHeap::heapify() {
    for (int index = size/2 - 1 ; index >= 0; index--) {
        siftDown(index);
    }
    for (int i = 0 ; i < size; i++) {
        position[array[i].second] = i;
    }
}

void MinHeap::decreaseKey(int pos, double newValue) {
    int array_index = position[pos];
    array[array_index].first = newValue;
    siftUp(array_index);
}


double MinHeap::operator[](int pos) {
    return array[position[pos]].first;
}