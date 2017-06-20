/* ./tracer/tracer.h
 * by kimht
 */
#ifndef _TRACER_TRACER_H_
#define _TRACER_TRACER_H_

//////////////
/// include
//////////////
#include "../base/base.h"
#include "../logger/logger.h"

/////////////
/// define
/////////////
#define FAILED_TO_START_TRACE_PROGRAM       -1
#define SUCCESS_TO_START_TRACE_PROGRAM      0

#define FAILED_TO_REPEAT_SINGLE_STEP        -1
#define SUCCESS_TO_REPEAT_SINGLE_STEP       0

#define FAILED_TO_HANDER_SINGLE_STEP        -1
#define SUCCESS_TO_HANDER_SINGLE_STEP       0

#define FAILED_TO_GET_REGISTER      -1
#define SUCCESS_TO_GET_REGISTER     0

#define FAILED_TO_IDENTIFY_BIT_MODE     -1
#define SUCCESS_TO_IDENTIFY_BIT_MODE    0

#define MODE_I386       0
#define MODE_X86_64     1

////////////
/// class
////////////
class Tracer {
public:
    static Tracer *GetInstance();
    int StartTracingProgram(const char *path, char *const argv[], char *const envp[]);

private:
    static Tracer *instance;
    string program_path_;
    pid_t child_pid_;
    int bit_mode_;

    Tracer();
    int IdentifyBitMode(const char *bin_path);
    int RepeatSingleStep();
    int HandlerSingleStep();
};

#endif

