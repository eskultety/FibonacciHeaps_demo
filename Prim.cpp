#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <unistd.h>
#include "Prim.h"
#include "FibHeap.h"
#include "interface.h"

#ifdef WITH_GUI
    #define MILLISEC 1000 // 1 second
    #define MUTEX_AC(s, p) _mutexAcquire(&s, &p);

    static inline void _mutexAcquire(unsigned char *s, bool *p)
    {
        prim_mtx.lock();
            *s = speed;
            *p = pause_execution;
        prim_mtx.unlock();
    }
#endif  // WITH_GUI

using namespace std;

int
Prim::PrimMinSpanningTree(unsigned (*weight)(unsigned u, unsigned v),
                          unsigned root)
{
    #ifdef WITH_GUI
    bool pause;
    unsigned char sp;
    #endif

    int ret = -1;
    int w = -1;
    unsigned i = 0;
    FibNodePtr u = nullptr;
    FibNodePtr v = nullptr;
    vector<FibNodePtr> pi(adj.size(), NULL);   // predecessor array

    // properly initialize AdjList
    for (i = 0; i < this->adj.size(); i++) {
    #ifdef WITH_GUI
        MUTEX_AC(sp, pause);
        this_thread::sleep_for(chrono::milliseconds(MILLISEC / sp));
    #endif


        this->adj[i][0]->key = INT_MAX;
        pi[this->adj[i][0]->id] = NULL;
    }

    #ifdef WITH_GUI
        MUTEX_AC(sp, pause);
        this_thread::sleep_for(chrono::milliseconds(MILLISEC / sp));
    #endif

    // r.key = 0
    this->adj[root][0]->key = 0;

    #ifdef WITH_GUI
        MUTEX_AC(sp, pause);
        this_thread::sleep_for(chrono::milliseconds(MILLISEC / sp));
    #endif

    // Q = V
    for (i = 0; i < this->adj.size(); i++) {
        if (fib_heap->FibInsertNode(adj[i][0]) < 0) {
            //cerr << "cannot insert node into heap\n";
            throw PrimException("cannot insert node into the heap");
            goto cleanup;
        }
    }

    #ifdef WITH_GUI
        MUTEX_AC(sp, pause);
        this_thread::sleep_for(chrono::milliseconds(MILLISEC / sp));
    #endif

    // find minimum spanning tree (set of edges)
    while (!fib_heap->FibIsEmpty()) {
        #ifdef WITH_GUI
            MUTEX_AC(sp, pause);
            this_thread::sleep_for(chrono::milliseconds(MILLISEC / sp));
        #endif

        // u <- Extract-Min(Q)
        u = fib_heap->FibExtractMin();
        if (!u) {
            cerr << "cannot extract minimum\n"; 
            goto cleanup;
        }

        // if pi[u] then A = A U (u, pi[u])
        if (pi[u->id]) {
            this->min_spanning_tree.push_back(make_tuple(u->id,
                                                         pi[u->id]->id));
            this->mst_cost += weight(u->id,pi[u->id]->id);
        }

        // for each v in Adj[u]
        for (i = 1; i < this->adj[u->id].size(); i++) {
            v = this->adj[u->id][i];
            #ifdef WITH_GUI
                MUTEX_AC(sp, pause);
                this_thread::sleep_for(chrono::milliseconds(MILLISEC / sp));
            #endif

            unsigned uid = u->id;
            unsigned vid = v->id;
            if ((v = fib_heap->FibFindNode(vid)) &&
                 (w = weight(uid, vid)) < v->key) {
                pi[vid] = u;
                fib_heap->FibDecreaseKey(v, w);
            }
        }
    }

    ret = 0;
 cleanup:
    return ret;
}

Prim::Prim()
{
    try {
        this->fib_heap = new FibHeap;
    }

    catch (bad_alloc&) {
        /* log error into the terminal */
        cerr << "FibHeap: cannot create Fibonacci heap\n";
        throw;
    }
}

Prim::~Prim()
{
    for (unsigned i = 0; i < this->adj.size(); i++) {
        FibNodePtr node = this->adj[i][0];
        delete node;
    }
    
    delete this->fib_heap;
}

FibNodePtr
Prim::PrimAddVertex()
{
    FibNodePtr node = fib_heap->FibCreateNode();
    if (node) {
        this->adj.push_back(AdjNodeEdges());
        this->adj[node->id].push_back(node);
    }

    return node;
}

int
Prim::PrimAddEdge(unsigned u, unsigned v)
{
    if (u >= adj.size() || v >= adj.size()) {
        cerr << "bad edge\n";
        return -1;
    }

    /* add v into u's adjacency list */
    this->adj[u].push_back(adj[v][0]);

    /* add u into v's adjacency list */
    this->adj[v].push_back(adj[u][0]);
    return 0;
}

