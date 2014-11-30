#ifndef __INTERNAL__
#define __INTERNAL__

#include <climits>
#include <vector>
#define SWAP(x, y) _swap(&x, &y)

template <typename T>
void _swap (T *a, T *b) {
    if (*a == *b)
        return;
    T *tmp = a;
    *a = *b;
    *b = *tmp;
}

typedef struct FibNode *FibNodePtr;
typedef struct AdjItem *AdjItemPtr;
typedef std::vector<AdjItemPtr> *AdjListPtr;    // adjacency list
typedef class FibonacciHeap FibonacciHeapPtr;

class FibonacciHeap;

struct FibNode {
    unsigned id;
    FibNodePtr parent;
    FibNodePtr child;   // children doubly-linked list
    FibNodePtr left, right;  // siblings doubly-linked list
    int key;
    unsigned degree;    // number of children
    bool mark;          // whether X has lost child since X was made a child
};

struct AdjItem {
    FibNodePtr data;
    AdjItemPtr next;
};

#endif // __INTERNAL__
