#ifndef __INTERNAL_H__
#define __INTERNAL_H__

#include <climits>
#include <vector>
#include <tuple>
#include <mutex>
#define SWAP(x, y) _swap(&x, &y)

template <typename T>
void _swap (T *a, T *b) {
    if (*a == *b)
        return;
    T tmp = *a;
    *a = *b;
    *b = tmp;
}

typedef struct FibNode *FibNodePtr;
typedef std::vector<std::tuple<int, int>> EdgeSet;   // min spanning tree
typedef std::vector<FibNodePtr> AdjNodeEdges;
typedef std::vector<AdjNodeEdges> AdjList;    // adjacency list

extern unsigned char speed; // algorithm speed limiter
extern bool pause_execution; // if we want to step the execution
extern std::mutex prim_mtx;

struct FibNode {
    unsigned id;
    FibNodePtr parent;
    FibNodePtr child;   // children doubly-linked list
    FibNodePtr left, right;  // siblings doubly-linked list
    int key;
    unsigned degree;    // number of children
    bool mark;          // whether X has lost child since X was made a child
};

#endif // __INTERNAL_H_
