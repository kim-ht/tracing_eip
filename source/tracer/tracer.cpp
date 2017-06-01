/* ./tracer/tracer.cpp
 * by kimht
 */
#include "./tracer.h"

/* public */
Tracer::Tracer() {
    child_pid_ = -1;
    rips_.clear();
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
    /* wait until done executing a code  */
    int status;
    waitpid(child_pid_, &status, WUNTRACED);
    if ( WIFSIGNALED(status) || WIFEXITED(status) ) {
        return FAILED_TO_STEP_INTO;
    }
    /* check if the tracee is crashed */
    siginfo_t siginfo;
    if ( ptrace(PTRACE_GETSIGINFO, child_pid_, NULL, &siginfo) == -1 ) {
        rips_.push_back(regs_.rip);
        return FAILED_TO_STEP_INTO;
    }
    int signo = siginfo.si_signo;
    if ( signo == SIGILL || signo == SIGSEGV || signo == SIGFPE || signo == SIGCHLD ) {
        return FAILED_TO_STEP_INTO;
    }
    /* handle before step into */
    if ( HandleStepInto() == FAILED_TO_HANDLE_STEP_INTO ) {
        return FAILED_TO_STEP_INTO;
    }
    /* step into */
    if ( ptrace(PTRACE_SINGLESTEP, child_pid_, NULL, NULL) == -1 ) {
        return FAILED_TO_STEP_INTO;
    }
    return SUCCESS_TO_STEP_INTO;
}

void Tracer::PrintRIPs() {
    for ( vector<long>::iterator tmp = rips_.begin(); tmp != rips_.end(); ++tmp ) {
        cout << "0x" << hex << *tmp << endl;
    }
}

/* private */
int Tracer::HandleStepInto() {
    /* get current registers */
    if ( ptrace(PTRACE_GETREGS, child_pid_, NULL, &regs_) == -1 ) {
        return FAILED_TO_HANDLE_STEP_INTO;
    }
    /* push current rip to rips_ vector */
    rips_.push_back(regs_.rip);
    return SUCCESS_TO_HANDLE_STEP_INTO;
}

