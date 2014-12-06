#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <cstdio>
#include "Prim.h"
#include "FibHeap.h"

#ifdef WITH_GUI
#include "interface.h"
#endif

using namespace std;

int
Prim::PrimMinSpanningTree(int (*weight)(unsigned u, unsigned v),
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
    vector<FibNodePtr> pi(this->adj.size(), NULL);   // predecessor array

    if (!this->adj.size()) {
        throw PrimException(fmtError("failed to find minimum spanning tree:"
                                     " no vertices given"));
        goto cleanup;
    }
    if (!this->edges.size()) {
        throw PrimException(fmtError("failed to find minimum spanning tree:"
                                     " no edges given"));
        goto cleanup;
    }
    if (root >= this->adj.size() || !this->adj[root][0]) {
        char buf[100];
        sprintf(buf,"failed to minimum spannig tree: root '%d' does not"
                " exist", root);
        throw PrimException(fmtError(buf));
        goto cleanup;
    }

    /* properly initialize Fibonacci heap */
    for (i = 0; i < this->adj.size(); i++) {
        if (!this->adj[i][0])
            continue;
        this->adj[i][0]->key = INT_MAX;
        pi[this->adj[i][0]->id] = NULL;
        #ifdef WITH_GUI // finished init
            MUTEX_AC(sp, pause);
            this_thread::sleep_for(chrono::milliseconds(MILLISEC / sp));
            sigEvent(SIG_PRIM_STEP_FINISHED);
            while (!ready)
                cv.wait(u_lock);
            if (sim_terminate)
                return 0;
            ready = false;
        #endif
    }

    /* r.key = 0 */
    this->adj[root][0]->key = 0;
    #ifdef WITH_GUI // finished r.key = 0
        MUTEX_AC(sp, pause);
        this_thread::sleep_for(chrono::milliseconds(MILLISEC / sp));
        sigEvent(SIG_PRIM_STEP_FINISHED);
        while (!ready)
            cv.wait(u_lock);
        if (sim_terminate)
            return 0;
        ready = false;
    #endif

    /* Q <-- V */
    for (i = 0; i < this->adj.size(); i++) {
        if (!this->adj[i][0])
            continue;
        if (fib_heap->FibInsertNode(this->adj[i][0]) < 0) {
            char buf[100];
            sprintf(buf, "failed to insert node %d into fibonacci heap",
                    adj[i][0]->id);
            throw PrimException(fmtError(buf));
            goto cleanup;
        }
    }
    #ifdef WITH_GUI // finished Q <-- V
        MUTEX_AC(sp, pause);
        this_thread::sleep_for(chrono::milliseconds(MILLISEC / sp));
        sigEvent(SIG_PRIM_STEP_FINISHED);
        while (!ready)
            cv.wait(u_lock);
        if (sim_terminate)
            return 0;
        ready = false;
    #endif

    /* find minimum spanning tree (set of edges) */
    while (!fib_heap->FibIsEmpty()) {

        /* u <- Extract-Min(Q) */
        u = fib_heap->FibExtractMin();
        if (!u) {
            throw PrimException(fmtError("failed to extract minimal"
                                         " value node"));
            goto cleanup;
        }
        #ifdef WITH_GUI // finished Extract_miin(Q)
            MUTEX_AC(sp, pause);
            this_thread::sleep_for(chrono::milliseconds(MILLISEC / sp));
            sigEvent(SIG_PRIM_STEP_FINISHED);
            while (!ready)
                cv.wait(u_lock);
            if (sim_terminate)
                return 0;
            ready = false;
        #endif


        /* if pi[u] then A = A U (u, pi[u]) */
        if (pi[u->id]) {
            this->min_spanning_tree.push_back(make_tuple(u->id,
                                                         pi[u->id]->id));
            this->mst_cost += weight(u->id,pi[u->id]->id);
        }
        #ifdef WITH_GUI // finished A U (u, pi[u])
            MUTEX_AC(sp, pause);
            this_thread::sleep_for(chrono::milliseconds(MILLISEC / sp));
            sigEvent(SIG_PRIM_STEP_FINISHED);
            while (!ready)
                cv.wait(u_lock);
            if (sim_terminate)
                return 0;
            ready = false;
        #endif

        /* for each v in Adj[u] */
        for (i = 1; i < this->adj[u->id].size(); i++) {
            v = this->adj[u->id][i];
            unsigned uid = u->id;
            unsigned vid = v->id;

            /* if v in Q and w(u,v) < key[v] */
            if ((v = fib_heap->FibFindNode(vid)) &&
                 (w = weight(uid, vid)) < v->key) {
                #ifdef WITH_GUI // finished if condition
                    MUTEX_AC(sp, pause);
                    this_thread::sleep_for(chrono::milliseconds(MILLISEC / sp));
                    sigEvent(SIG_PRIM_STEP_FINISHED);
                    while (!ready)
                        cv.wait(u_lock);
                    if (sim_terminate)
                        return 0;
                    ready = false;
                #endif

                /* pi[v] <-- u; key[v] <-- w(u,v) */
                pi[vid] = u;
                fib_heap->FibDecreaseKey(v, w);
                #ifdef WITH_GUI // finished assignements
                    MUTEX_AC(sp, pause);
                    this_thread::sleep_for(chrono::milliseconds(MILLISEC / sp));
                    sigEvent(SIG_PRIM_STEP_FINISHED);
                    while (!ready)
                        cv.wait(u_lock);
                    if (sim_terminate)
                        return 0;
                    ready = false;
                #endif
            }
        }
    }

    ret = 0;
 cleanup:
    this->status_finished = true;
    return ret;
}

Prim::Prim()
{
    try {
        this->fib_heap = new FibHeap;
    }

    catch (bad_alloc&) {
        throw PrimException(fmtError("failed to create Fibonacci heap"));
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
Prim::PrimAddVertex(unsigned id)
{
    FibNodePtr node = nullptr;
    if (id == UINT_MAX)
        node = fib_heap->FibCreateNode();
    else
        node = fib_heap->FibCreateNode(id);
    if (node) {
        /* if for some reason user randomly chooses ID, we fill the Nodeset
         * with blanks
         */
        if (id > this->next_id) {
            for (unsigned i = 0; i < id - this->next_id; i++, this->next_id++) {
                this->adj.push_back(AdjNodeEdges());
                this->adj[this->next_id].push_back(nullptr);
            }
        } else if (id < this->next_id) {
            this->adj[id][0] = node;
            return node;
        }

        this->adj.push_back(AdjNodeEdges());
        this->adj[node->id].push_back(node);
        this->next_id = id + 1;
    }

    return node;
}

int
Prim::PrimAddEdge(unsigned u, unsigned v)
{

    if (u >= this->next_id || v >= this->next_id) {
        char buf[100];
        sprintf(buf, "failed to insert edge (%d,%d): invalid edge", u,v);
        throw PrimException(fmtError(buf));
        return -1;
    }

    /* check for conflicting edges */
    for (vector<tuple<int, int>>::iterator it = this->edges.begin();
         it != this->edges.end();
         ++it) {
        unsigned my_u, my_v;
        tie(my_u, my_v) = *it;  /* untie edge to separate nodes */
        if ((my_u == u && my_v == v) || (my_u == v && my_v == u)) {
            char buf [100];
            sprintf(buf,
                    "failed to insert edge (%d,%d): edge already exists",
                    u,v);
            throw PrimException(fmtError(buf));
            return -1;
        }
    }

    /* insert edge into list of all edges, so we can later check
     * if user tries to insert reverted edge as well which must fail
     * with error, as Prim's algorithm works only for undirected graphs
     */
    this->edges.push_back(make_tuple(u,v));

    /* add v into u's adjacency list */
    this->adj[u].push_back(adj[v][0]);

    /* add u into v's adjacency list */
    this->adj[v].push_back(adj[u][0]);
    return 0;
}

