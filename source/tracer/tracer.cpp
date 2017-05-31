/* ./tracer/tracer.cpp
 * by kimht
 */
#include "./tracer.h"

/* public */
Tracer::Tracer() {
    child_pid_ = -1;
    orig_rax_ = 0;
    orig_rip_ = 0;
}

Tracer::~Tracer() {
}

int Tracer::RunProgram(const char *path, char *const argv[], char *const envp[]) {
    pid_t pid = fork();
    /* failed to fork() */
    if ( pid == -1 ) {
        perror("Tracer::RunProgram() error: ");
        return FAILED_TO_RUN_PROGRAM;
    /* child */
    } else if ( pid == 0 ) {
        ptrace(PTRACE_TRACEME, NULL, NULL, NULL);
        if ( execvpe(path, argv, envp) == -1 ) {
            perror("Tracer::RunProgram() error: ");
            return FAILED_TO_RUN_PROGRAM;
        }
    /* parent */
    } else {
        wait(NULL);
        child_pid_ = pid;
        ptrace(PTRACE_SINGLESTEP, child_pid_, NULL, NULL);
    }
    return SUCCESS_TO_RUN_PROGRAM;
}

int Tracer::StepInto() {
    int status;
    /* wait until done executing a code  */
    waitpid(child_pid_, &status, WUNTRACED);
    if ( WIFEXITED(status) ) {
        return FAILED_TO_STEP_INTO;
    }
    /* handle before step into */
    if ( HandleStepInto() == FAILED_TO_HANDLE_STEP_INTO ) {
        return FAILED_TO_STEP_INTO;
    }
    /* step into */
    ptrace(PTRACE_SINGLESTEP, child_pid_, NULL, NULL);
    return SUCCESS_TO_STEP_INTO;
}



/* private */
long Tracer::GetCurrentRAX() {
    return orig_rax_;
}

long Tracer::GetCurrentRIP() {
    return orig_rip_;
}

int Tracer::HandleStepInto() {
    orig_rip_ = ptrace(PTRACE_PEEKUSER, child_pid_, 8 * RIP, NULL);
    if ( errno ) {
        return FAILED_TO_HANDLE_STEP_INTO;
    }
    cout << "current pc: " << hex << orig_rip_ << endl;
    return SUCCESS_TO_HANDLE_STEP_INTO;
}

