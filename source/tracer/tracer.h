/* ./tracer/tracer.h
 * by kimht
 */
#ifndef _TRACER_TRACER_H_
#define _TRACER_TRACER_H_

/* include */
#include "../base/base.h"

/* define */
#define FAILED_TO_RUN_PROGRAM       0
#define SUCCESS_TO_RUN_PROGRAM      1 

#define FAILED_TO_STEP_INTO     0
#define SUCCESS_TO_STEP_INTO    1

#define FAILED_TO_HANDLE_STEP_INTO      0
#define SUCCESS_TO_HANDLE_STEP_INTO     1

#define FAILED_TO_GET_REGISTER      0
#define SUCCESS_TO_GET_REGISTER     1

/* class */
class Tracer {
public:
    Tracer();
    ~Tracer();
    int RunProgram(const char *path, char *const argv[], char *const envp[]);
    int StepInto();
    void PrintRIPs();

private:
    pid_t child_pid_;
    struct user_regs_struct regs_;
    vector<long> rips_;

    int HandleStepInto();
};

#endif

