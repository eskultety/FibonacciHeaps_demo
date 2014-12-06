#ifndef __INTERNAL_H__
#define __INTERNAL_H__

#include <climits>
#include <vector>
#include <tuple>
#include <mutex>
#include <cstdio>
#include <string>

#define fmtError(fmt) _fmterror(__func__, fmt)

static inline std::string _fmterror(const std::string func,
                                    const std::string msg)
{
    return func + ": " + msg;
}

typedef struct FibNode *FibNodePtr;
typedef std::vector<std::tuple<int, int>> EdgeSet;   // min spanning tree
typedef std::vector<FibNodePtr> AdjNodeEdges;
typedef std::vector<AdjNodeEdges> AdjList;    // adjacency list

#ifdef WITH_GUI
extern unsigned char speed; // algorithm speed limiter
extern bool pause_execution; // if we want to step the execution
extern std::mutex shared_mtx;   // protects GUI speed settings
extern std::mutx uni_mtx;  // used to acquire lock on conditional variable
#endif // WITH_GUI

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
