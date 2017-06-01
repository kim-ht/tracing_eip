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

#define FAILED_TO_HANDLE_DURING_STEP_INTO       0
#define SUCCESS_TO_HANDLE_DURING_STEP_INTO      1

#define FAILED_TO_GET_REGISTER      0
#define SUCCESS_TO_GET_REGISTER     1

#define FAILED_TO_START_LOGGING_RIP     0
#define SUCCESS_TO_START_LOGGING_RIP    1

#define FAILED_TO_END_LOGGING_RIP       0
#define SUCCESS_TO_END_LOGGING_RIP      1

/* class */
class Tracer {
public:
    Tracer();
    int TraceProgram(const char *path, char *const argv[], char *const envp[]);

private:
    string log_path_;
    pid_t child_pid_;
    ofstream log_file_;

    int StartLoggingRIP();
    void LogRIP(long rip);
    void EndLoggingRIP();
    int StepInto();
    int HandleDuringStepInto();
};

#endif

