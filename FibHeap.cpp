#include <iostream>
#include <cmath>
#include <string>
#include "FibHeap.h"

#ifdef WITH_GUI
#include "interface.h"
#endif

#define SWAP(x, y) _swap(&x, &y)

using namespace std;

template <typename T>
static void _swap (T *a, T *b)
{
    if (*a == *b)
        return;
    T tmp = *a;
    *a = *b;
    *b = tmp;
}

FibHeap::~FibHeap()
{
    if (min)
        FibDeleteHeap(min);
}

void
FibHeap::FibDeleteHeap(FibNodePtr x)
{
    FibNodePtr ptr = x;

    if (ptr) {
        do {
            FibNodePtr tmp = ptr;
            ptr = ptr->right;
            FibDeleteHeap(tmp->child);
            delete tmp;
        } while (ptr != x);
    }
}

inline void
FibHeap::FibMoveToRoot(FibNodePtr node)
{
    FibNodePtr min = this->min;

    node->left = min->left;
    node->right = min;
    min->left->right = node;
    min->left = node;
}

int
FibHeap::FibInsertNode(FibNodePtr node)
{
    if (!this->min) {
        this->min = node;
    } else {
        this->FibMoveToRoot(node);
        if (node->key < this->min->key)
            this->min = node;
    }

    this->numNodes++;
    #ifdef WITH_GUI
        syncGUI(SIG_FIB_STEP_FINISHED);
    #endif
    return 0;
}

FibNodePtr
FibHeap::FibCreateNode(unsigned my_id, int key)
{
    FibNodePtr node = new(nothrow) FibNode;
    if (node) {
        if (my_id == INT_MAX) {
            node->id = this->next_id++;
        } else {
            node->id = my_id;
            this->next_id = my_id + 1;
        }
        node->key = key;
        node->degree = 0;
        node->mark = 0;
        node->left = node;
        node->right = node;
        node->child = nullptr;  // explicit init due to valgrind
        node->parent = nullptr; // explicit init due to valgrind
    }

    return node;
}

FibNodePtr
FibHeap::FibExtractMin()
{
    FibNodePtr heap_min = this->min;
    FibNodePtr tmp = nullptr;

    if (heap_min) {
        FibNodePtr child = heap_min->child;
        if (child) {
            do {
                /* Properly splice out every child node and update all
                 * pointers. The only reason why it is necessary to update
                 * node's child pointer even though it is going to be removed
                 * as minimal, is because we need to synchronize with GUI and
                 * without proper update of child pointer, there's a risk
                 * of infinite recursion when rendering the nodes
                 */
                if (child == child->right)
                    heap_min->child = nullptr;
                else {
                    child->left->right = child->right;
                    child->right->left = child->left;
                    heap_min->child = child->right;
                }
                tmp = child->right;
                this->FibMoveToRoot(child);
                child->parent = nullptr;
                child = tmp;

                #ifdef WITH_GUI
                    syncGUI(SIG_FIB_STEP_FINISHED);
                #endif

            } while(heap_min->child);
        }

        /* remove minimum from the root list of H */
        heap_min->left->right = heap_min->right;
        heap_min->right->left = heap_min->left;

        if (heap_min == heap_min->right) {
            this->min = nullptr;

            #ifdef WITH_GUI
                syncGUI(SIG_FIB_STEP_FINISHED);
            #endif
        } else {
            this->min = heap_min->right;

            #ifdef WITH_GUI
                syncGUI(SIG_FIB_STEP_FINISHED);
            #endif
            this->FibConsolidate();
        }

        this->numNodes--;
    }

    return heap_min;
}

int
FibHeap::FibConsolidate()
{
    FibNodePtr ptr_x = this->min;
    FibNodePtr ptr_y = nullptr;
    double phi = ((1 + sqrt(5)) / 2); // golden ratio used for logarithm base
    int deg = -1;
    int max_degree =
        static_cast<int>(floor(
                               log(static_cast<double>(this->numNodes)) /
                               log(phi)));

    /* auxiliary array to keep track of roots according to their degrees */
    vector<FibNodePtr> ax_array(max_degree, nullptr);

    /* When looping through the rootlist, we have to keep track of nodes
     * already visited which is quite complicated with circular lists,
     * especially when the list changes on the go. For this reason, we use
     * a helper boolean vector which can tell us, if the given node has already
     * been procesed, thus we need to stop looping
     */
    vector<bool> nodes_visited(this->next_id, false);

    if (!this->min || this->min == this->min->right)
        return 0;

    do {
        deg = ptr_x->degree;

        /* mark current node as visited */
        nodes_visited[ptr_x->id] = true;
        while (ax_array[deg]) {
            ptr_y = ax_array[deg];
            if (ptr_x->key > ptr_y->key) {
                SWAP(ptr_x, ptr_y);
            }

            /* make y child of x */
            this->FibHeapLink(ptr_y, ptr_x);
            ax_array[deg] = nullptr;
            deg++;
        }

        ax_array[deg] = ptr_x;
        ptr_x = ptr_x->right;
    } while (!nodes_visited[ptr_x->id]);

    this->min = nullptr;

    /* create a new rootlist */
    for (unsigned i = 0; i < ax_array.size(); i++) {
        if (ax_array[i]) {
            if (!this->min) {

                /* create a root list for H containing jus A[i] */
                ax_array[i]->left = ax_array[i];
                ax_array[i]->right = ax_array[i];
                this->min = ax_array[i];

                #ifdef WITH_GUI
                    syncGUI(SIG_FIB_STEP_FINISHED);
                #endif

            } else {
                this->FibMoveToRoot(ax_array[i]);

                #ifdef WITH_GUI
                    syncGUI(SIG_FIB_STEP_FINISHED);
                #endif

                if (ax_array[i]->key < this->min->key) {
                    this->min = ax_array[i];

                    #ifdef WITH_GUI
                        syncGUI(SIG_FIB_STEP_FINISHED);
                    #endif
                }
            }
        }
    }

    return 0;
}

int
FibHeap::FibHeapLink(FibNodePtr y, FibNodePtr x)
{
    if (!x)
        return -1;

    FibNodePtr *childptr = &(x->child);

    /* remove y from the root list of H */
    y->right->left = y->left;
    y->left->right = y->right;
    y->left = y;
    y->right = y;
    y->parent = x;

    /* make y a child of x */
    if (!(*childptr))
        (*childptr) = y;

    (*childptr)->left->right = y;
    y->left = (*childptr)->left;
    (*childptr)->left = y;
    y->right = (*childptr);

    x->degree++;
    y->mark = false;

    return 0;
}

int
FibHeap::FibDecreaseKey(FibNodePtr x, int key)
{
    FibNodePtr y = nullptr;
    int ret = -1;
    char buf[100] = {0};

    if (!x) {
        sprintf(buf,"node %d does not exist", x->id);
        throw FibException(fmtError(buf));
        goto error;
    }

    if (key > x->key) {
        throw FibException(fmtError("key is greater than current key"));
        goto error;
    }

    x->key = key;
    y = x->parent;

    #ifdef WITH_GUI
        syncGUI(SIG_FIB_STEP_FINISHED);
    #endif

    if (y && (x->key < y->key)) {
        this->FibCut(x, y);
        this->FibCascadingCut(y);
    }

    if (x->key < this->min->key)
        this->min = x;

    #ifdef WITH_GUI
        syncGUI(SIG_FIB_STEP_FINISHED);
    #endif
    ret = 0;
 error:
    return ret;
}

int
FibHeap::FibCut(FibNodePtr x, FibNodePtr y)
{
    if (!x)
        return -1;

    // remove x from child list of y
    if (!(x == x->right)) {
        x->left->right = x->right;
        x->right->left = x->left;
        y->child = x->right;
    } else {
        y->child = nullptr;
    }

    #ifdef WITH_GUI
        syncGUI(SIG_FIB_STEP_FINISHED);
    #endif

    // add x to the root list of H
    this->FibMoveToRoot(x);

    #ifdef WITH_GUI
        syncGUI(SIG_FIB_STEP_FINISHED);
    #endif

    y->degree--;
    x->parent = nullptr;
    x->mark = false;

    #ifdef WITH_GUI
        syncGUI(SIG_FIB_STEP_FINISHED);
    #endif
    return 0;
}

int
FibHeap::FibCascadingCut(FibNodePtr y)
{
    FibNodePtr ptr = y->parent;

    if (!ptr)
        return -1;

    if (!(y->mark)) {
        y->mark = true;

        #ifdef WITH_GUI
            syncGUI(SIG_FIB_STEP_FINISHED);
        #endif

    } else {
        this->FibCut(y, ptr);
        this->FibCascadingCut(ptr);
    }

    return 0;
}

int
FibHeap::FibDeleteNode(FibNodePtr node)
{
    FibNodePtr retnode = nullptr;
    int ret = -1;
    if ((ret = this->FibDecreaseKey(node, INT_MIN)) < 0) {
        throw FibException(fmtError("failed to decrease key"));
        goto cleanup;
    }

    if (!(retnode = this->FibExtractMin())) {
        throw FibException(fmtError("failed to extract minimal node"));
        goto cleanup;
    }

    ret = 0;
 cleanup:
    delete retnode;
    return ret;
}

FibNodePtr
FibHeap::FibFindNode(unsigned id)
{
    FibNodePtr ret = nullptr;
    FibNodePtr ptr = this->min;

    ret = this->FibFindImpl(ptr, id);
    return ret;
}

FibNodePtr
FibHeap::FibFindImpl(FibNodePtr x, unsigned id)
{
    FibNodePtr ptr = x;
    FibNodePtr tmp = ptr;

    if (ptr) {
        do {
            if (ptr->id == id)
                return ptr;
            tmp = ptr;
            ptr = ptr->right;
            if ((tmp = this->FibFindImpl(tmp->child, id)))
                return tmp;
        } while (ptr != x);
    }

    return nullptr;
}

FibHeapPtr
FibUnion(FibHeap &heap1, FibHeap &heap2) {
    FibHeap *heap_ptr = nullptr;
    FibNodePtr h_min = nullptr;
    FibNodePtr h_min1 = heap1.min;
    FibNodePtr h_min2 = heap2.min;
    FibNodePtr tmp = nullptr;

    heap_ptr = new(nothrow) FibHeap;
    if (!heap_ptr) {
        throw FibHeap::FibException(fmtError("failed to allocate Fibonacci heap"));
        goto cleanup;
    }

    heap_ptr->min = h_min1;
    h_min = heap_ptr->min;

    // concatenate the root list of H2 with root list of H
    h_min->left->right = h_min2;
    tmp = h_min->left;
    h_min->left = h_min2->left;
    h_min2->left->right = h_min;
    h_min2->left = tmp;
    if (!h_min1 || (h_min2 && h_min2->key < h_min1->key))
        heap_ptr->min = h_min2;

    heap_ptr->numNodes = heap1.numNodes + heap2.numNodes;

    /* because we now have all the references in the new created heap,
     * we can remove the references from the original heaps, but still
     * leaving the caller with the responsibility to properly deallocate
     * all dynamic memory
     */
    heap1.min = nullptr;
    heap2.min = nullptr;
 cleanup:
    return heap_ptr;
}

