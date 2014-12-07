#ifndef __INTERFACE_H__
#define __INTERFACE_H__

#include <mutex>
#include <thread>
#include <condition_variable>

#define MILLISEC 1000 // 1 second

#define SIG_PRIM_STEP_FINISHED  0
#define SIG_FIB_STEP_FINISHED   1
#define SIG_FINISHED_ALL        2
#define SIG_MIN_EXTRACTED       3
#define SIG_MST_UPDATED         4
#define SIG_ERROR               5

typedef enum {RUN,STEP} runMode;

extern unsigned char speed; // algorithm speed limiter
extern std::condition_variable cv;
extern bool ready;  // signalizes if GUI ready for next update
extern bool sim_terminate;
extern runMode mode; // which mode of execution we do prefer
extern std::mutex shared_mtx;   // protects GUI speed settings
extern std::mutex uni_mtx;  // used to acquire lock on conditional variable

void sigEvent(int sig, unsigned u=UINT_MAX, unsigned v=UINT_MAX);
void syncGUI(int sig, unsigned u=UINT_MAX, unsigned v=UINT_MAX);

#endif // __INTERFACE_H__
