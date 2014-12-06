#ifndef __INTERFACE_H__
#define __INTERFACE_H__

#include <mutex>
#include <thread>
#include <condition_variable>

#define MILLISEC 1000 // 1 second
#define MUTEX_AC(s, p) _mutexAcquire(&s, &p);

#define SIG_PRIM_STEP_FINISHED  0
#define SIG_FIB_STEP_FINISHED   1
#define SIG_ERROR               2
#define SIG_FINISHED_ALL        3

extern unsigned char speed; // algorithm speed limiter
extern std::condition_variable cv;
extern bool ready;  // signalizes if GUI ready for next update
extern bool sim_terminate;
extern bool pause_execution; // if we want to step the execution
extern std::mutex shared_mtx;   // protects GUI speed settings
extern std::mutex uni_mtx;  // used to acquire lock on conditional variable
extern std::unique_lock<std::mutex> u_lock;


void sigEvent(unsigned);

static inline void _mutexAcquire(unsigned char *s, bool *p)
{
    shared_mtx.lock();
        *s = speed;
        *p = pause_execution;
    shared_mtx.unlock();
}

#endif // __INTERFACE_H__
