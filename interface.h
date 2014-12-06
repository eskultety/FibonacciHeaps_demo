#ifndef __INTERFACE_H__
#define __INTERFACE_H__

#define MILLISEC 1000 // 1 second
#define MUTEX_AC(s, p) _mutexAcquire(&s, &p);

extern unsigned char speed; // algorithm speed limiter
extern bool ready;  // signalizes if GUI ready for next update
extern bool pause_execution; // if we want to step the execution
extern std::mutex shared_mtx;   // protects GUI speed settings
extern std::mutex uni_mtx;  // used to acquire lock on conditional variable

typedef enum
{
    SIG_PRIM_STEP_FINISHED,
    SIG_FIB_STEP_FINISHED,
    SIG_ERROR,
    SIG_LAST
} sigTypes;

void sigStepFinished();

static inline void _mutexAcquire(unsigned char *s, bool *p)
{
    shared_mtx.lock();
        *s = speed;
        *p = pause_execution;
    shared_mtx.unlock();
}

#endif // __INTERFACE_H__
