/* ./tracer/tracer.h
 * by kimht
 */
#include "../base/base.h"

#define FAILED_TO_RUN_PROGRAM       0
#define SUCCESS_TO_RUN_PROGRAM      1 

#define FAILED_TO_STEP_INTO     0
#define SUCCESS_TO_STEP_INTO    1

#define FAILED_TO_HANDLE_STEP_INTO      0
#define SUCCESS_TO_HANDLE_STEP_INTO     1

class Tracer {
public:
    Tracer();
    ~Tracer();
    int RunProgram(const char *path, char *const argv[], char *const envp[]);
    int StepInto();

private:
    pid_t child_pid_;
    long orig_rax_;
    long orig_rip_;

    long GetCurrentRAX();
    long GetCurrentRIP();
    int HandleStepInto();
};

