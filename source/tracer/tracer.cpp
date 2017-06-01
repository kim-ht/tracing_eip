/* ./tracer/tracer.cpp
 * by kimht
 */
#include "./tracer.h"

/* public */
Tracer::Tracer() {
    log_path_ = "";
    child_pid_ = -1;
}

int Tracer::TraceProgram(const char *path, char *const argv[], char *const envp[]) {
    pid_t pid = fork();
    /* failed to fork() */
    if ( pid == -1 ) {
        perror("Tracer::RunProgram() failed:  ");
        return FAILED_TO_RUN_PROGRAM;
    /* child */
    } else if ( pid == 0 ) {
        if ( ptrace(PTRACE_TRACEME, NULL, NULL, NULL) == -1 ) {
            perror("Tracer::RunProgram() failed:  ");
            return FAILED_TO_RUN_PROGRAM;
        }
        if ( execvpe(path, argv, envp) == -1 ) {
            perror("Tracer::RunProgram() failed:  ");
            return FAILED_TO_RUN_PROGRAM;
        }
    /* parent */
    } else {
        if ( wait(NULL) == -1 ) {
            perror("Tracer::RunProgram() failed:  ");
            return FAILED_TO_RUN_PROGRAM;
        }
        child_pid_ = pid;
        if ( ptrace(PTRACE_SINGLESTEP, child_pid_, NULL, NULL) == -1 ) {
            perror("Tracer::RunProgram() failed:  ");
            return FAILED_TO_RUN_PROGRAM; 
        }
        /* start logging/tracing rip */
        log_path_ = (string)path + ".rip";
        if ( StartLoggingRIP() == FAILED_TO_START_LOGGING_RIP ) {
            perror("Tracer::RunProgram() failed:  ");
            return FAILED_TO_RUN_PROGRAM;
        }
        /* step into loop */
        while ( StepInto() != FAILED_TO_STEP_INTO );
        EndLoggingRIP();
    }
    return SUCCESS_TO_RUN_PROGRAM;
}

/* private */
int Tracer::StartLoggingRIP() {
    /* create log file that its name is tracee's path.rip */
    log_file_.open(log_path_.c_str(), ios::out | ios::binary);
    if ( log_file_.is_open() == false ) {
        perror("Tracer::StartLoggingRIP() failed:  ");
        return FAILED_TO_START_LOGGING_RIP;
    }
    return SUCCESS_TO_START_LOGGING_RIP;
}

void Tracer::LogRIP(long rip) {
    log_file_.write((char *)&rip, sizeof(long));
}

void Tracer::EndLoggingRIP() {
    log_file_.close();
}

int Tracer::StepInto() {
    /* wait until done executing a code  */
    int status;
    waitpid(child_pid_, &status, WUNTRACED);
    if ( WIFSIGNALED(status) || WIFEXITED(status) ) {
        perror("Tracer::StepInto() failed:  ");
        return FAILED_TO_STEP_INTO;
    }
    /* check if the tracee is crashed */
    siginfo_t siginfo;
    if ( ptrace(PTRACE_GETSIGINFO, child_pid_, NULL, &siginfo) == -1 ) {
        perror("Tracer::StepInto() failed:  ");
        return FAILED_TO_STEP_INTO;
    }
    int signo = siginfo.si_signo;
    if ( signo == SIGILL || signo == SIGSEGV || signo == SIGFPE || signo == SIGCHLD ) {
        perror("Tracer::StepInto() failed:  ");
        return FAILED_TO_STEP_INTO;
    }
    /* handle before step into like code hooking */
    if ( HandleDuringStepInto() == FAILED_TO_HANDLE_DURING_STEP_INTO ) {
        perror("Tracer::StepInto() failed:  ");
        return FAILED_TO_STEP_INTO;
    }
    /* step into */
    if ( ptrace(PTRACE_SINGLESTEP, child_pid_, NULL, NULL) == -1 ) {
        perror("Tracer::StepInto() failed:  ");
        return FAILED_TO_STEP_INTO;
    }
    return SUCCESS_TO_STEP_INTO;
}

int Tracer::HandleDuringStepInto() {
    /* get current registers and log rip */
    struct user_regs_struct regs;
    if ( ptrace(PTRACE_GETREGS, child_pid_, NULL, &regs) == -1 ) {
        perror("Tracer::HandleDuringStepInto() failed:  ");
        return FAILED_TO_HANDLE_DURING_STEP_INTO;
    }
    LogRIP(regs.rip);
    /* append rip to it's rip tracing log */
    return SUCCESS_TO_HANDLE_DURING_STEP_INTO;
}

