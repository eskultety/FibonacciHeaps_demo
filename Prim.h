#ifndef __PRIM_H__
#define __PRIM_H__
#include <iostream>
#include "internal.h"
#include "FibHeap.h"

class Prim {
    private:
        AdjList adj;
        EdgeSet edges;
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

        class PrimException: public std::exception
        {
            private:
                std::string msg;
            public:
                PrimException(const std::string& message)
                {
                    msg = std::string("error: ") + message + "\n";
                }
                virtual const char* what() const throw() { return msg.c_str(); }
        };
};

#endif  // __PRIM_H__
