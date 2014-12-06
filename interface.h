
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
