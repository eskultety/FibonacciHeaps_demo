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
        FibHeapPtr fib_heap = nullptr;
        int mst_cost = 0;
        unsigned next_id = 0;
        bool status_finished = false;
    public:
        Prim();
        ~Prim();
        FibNodePtr PrimAddVertex(unsigned id=UINT_MAX);
        int PrimAddEdge(unsigned u, unsigned v);
        int PrimMinSpanningTree(int (*weight)(unsigned u, unsigned v),
                                unsigned root);
        int PrimGetMSTCost(void) { return mst_cost; }
        EdgeSet& PrimGetMST(void) { return min_spanning_tree; }

        #ifdef WITH_GUI
        bool PrimGetStatus(void) { return status_finished; }
        FibNodePtr PrimGetHeapMin(void);\
        std::vector<FibNodePtr>* PrimGetAx(void);
        #endif


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
