#ifndef __PRIM_H__
#define __PRIM_H__
#include <iostream>
#include "internal.h"
#include "FibHeap.h"

class Prim {
    private:
        AdjList adj;
        EdgeSet min_spanning_tree;
        FibHeapPtr fib_heap;
        int mst_cost = 0;
    public:
        Prim();
        ~Prim();
        static const int random[5][5];
        FibNodePtr PrimAddVertex();
        int PrimAddEdge(unsigned u, unsigned v);
        int PrimMinSpanningTree(int (*weight)(unsigned u, unsigned v),
                                unsigned root);
        int getPrimMSTCost(void) { return mst_cost; }
};

#endif  // __PRIM_H__
