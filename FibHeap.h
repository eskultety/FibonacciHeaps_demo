#ifndef __FIBHEAP_H__
#define __FIBHEAP_H__

#include <exception>
#include <string>
#include <typeinfo>
#include "internal.h"

typedef class FibHeap *FibHeapPtr;

class FibHeap
{
    private:
        FibNodePtr min; // root of a tree containing minimum key
        unsigned numNodes;   // total number of nodes in the heap

        #ifdef WITH_GUI
        std::vector<FibNodePtr> *ax = nullptr;
        #endif
        int FibConsolidate();
        int FibHeapLink(FibNodePtr y, FibNodePtr x);
        int FibCascadingCut(FibNodePtr y);
        int FibCut(FibNodePtr x, FibNodePtr y);
        FibNodePtr FibFindImpl(FibNodePtr x, unsigned id);
        void FibDeleteHeap(FibNodePtr x);
        void FibMoveToRoot(FibNodePtr x);

    public:
        #ifdef WITH_GUI
        std::vector<FibNodePtr> *FibGetAX(void) { return ax; }
        #endif
        FibHeap() { numNodes = 0; min = nullptr; }
        ~FibHeap();
        bool FibIsEmpty() { return min == nullptr; }
        FibNodePtr FibGetMin() { return min; }
        FibNodePtr FibExtractMin();
        int FibDecreaseKey(FibNodePtr, int key);
        int FibDeleteNode(FibNodePtr node);
        int FibInsertNode(FibNodePtr node);
        FibNodePtr FibFindNode(unsigned id);
        FibNodePtr FibCreateNode(unsigned id=INT_MAX, int key=INT_MIN);
        friend FibHeapPtr
            FibUnion (FibHeap &h1, FibHeap &h2);

        class FibException: public std::exception
        {
            private:
                std::string msg;
            public:
                FibException(const std::string& message)
                {
                    msg = std::string("error: ") + message + "\n";
                }
                virtual const char* what() throw() { return msg.c_str(); }
        };
};

#endif  // __FIBHEAP_H__
