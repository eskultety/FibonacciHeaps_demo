#ifndef __FIBHEAP_H__
#define __FIBHEAP_H__

#include "internal.h"

typedef class FibHeap *FibHeapPtr;

class FibHeap
{
    private:
        FibNodePtr min; // root of a tree containing minimum key
        unsigned numNodes;   // total number of nodes in the heap

        int FibConsolidate();
        int FibHeapLink(FibNodePtr y, FibNodePtr x);
        int FibCascadingCut(FibNodePtr y);
        int FibCut(FibNodePtr x, FibNodePtr y);
        FibNodePtr FibFindImpl(FibNodePtr x, unsigned id);
        void FibDeleteHeap(FibNodePtr x);
        void FibMoveToRoot(FibNodePtr x);

    public:
        FibHeap() { numNodes = 0; min = nullptr; }
        ~FibHeap();
        bool FibIsEmpty() { return min == nullptr; }
        FibNodePtr FibGetMin() { return min; }
        FibNodePtr FibExtractMin();
        int FibDecreaseKey(FibNodePtr, int key);
        int FibDeleteNode(FibNodePtr node);
        int FibInsertNode(FibNodePtr node);
        FibNodePtr FibFindNode(unsigned id);
        FibNodePtr FibCreateNode(int key=INT_MIN);
        friend FibHeapPtr
            FibUnion (FibHeap &h1, FibHeap &h2);
};

#endif  // __FIBHEAP_H__
