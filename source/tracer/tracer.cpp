/* ./tracer/tracer.cpp
 * by kimht
 */
#include "./tracer.h"

/* public */
Tracer::Tracer() {
    child_pid_ = -1;
    bit_mode_ = MODE_I386;
    log_path_ = "";
}

int Tracer::TraceProgram(const char *path, char *const argv[], char *const envp[]) {
    /* identify 32bit or 64bit binary */
    if ( IdentifyBitMode(path) == FAILED_TO_IDENTIFY_BIT_MODE ) {
        perror("Tracer::RunProgram() failed");
        cout << "  --> IdentifyBitMode(path) == FAILED_TO_IDENTIFY_BIT_MODE" << endl;
        return FAILED_TO_RUN_PROGRAM;
    }
    pid_t pid = fork();
    /* failedto fork() */
    if ( pid == -1 ) {
        perror("Tracer::RunProgram() failed");
        cout << " --> failed to fork()" << endl;
        return FAILED_TO_RUN_PROGRAM;
    /* child */
    } else if ( pid == 0 ) {
        if ( ptrace(PTRACE_TRACEME, NULL, NULL, NULL) == -1 ) {
            perror("Tracer::RunProgram() failed");
            cout << "  --> ptrace(PTRACE_TRACEME, NULL, NULL, NULL) == -1" << endl;
            return FAILED_TO_RUN_PROGRAM;
        }
        if ( execvpe(path, argv, envp) == -1 ) {
            perror("Tracer::RunProgram() failed");
            cout << "  --> execvpe(path, argv, envp) == -1" << endl;
            return FAILED_TO_RUN_PROGRAM;
        }
    /* parent */
    } else {
        if ( wait(NULL) == -1 ) {
            perror("Tracer::RunProgram() failed");
            cout << "  --> wait(NULL) == -1" << endl;
            return FAILED_TO_RUN_PROGRAM;
        }
        child_pid_ = pid;
        if ( ptrace(PTRACE_SINGLESTEP, child_pid_, NULL, NULL) == -1 ) {
            perror("Tracer::RunProgram() failed");
            cout << "  --> ptrace(PTRACE_SINGLESTEP, child_pid_, NULL, NULL) == -1" << endl;
            return FAILED_TO_RUN_PROGRAM; 
        }
        /* start logging/tracing rip */
        log_path_ = (string)path + ".rip";
        if ( StartLoggingRIP() == FAILED_TO_START_LOGGING_RIP ) {
            perror("Tracer::RunProgram() failed");
            cout << "  --> StartLoggingRIP() == FAILED_TO_START_LOGGING_RIP" << endl;
            return FAILED_TO_RUN_PROGRAM;
        }
        /* step into loop */
        while ( StepInto() != FAILED_TO_STEP_INTO );
        EndLoggingRIP();
    }
    return SUCCESS_TO_RUN_PROGRAM;
}

/* private */
int Tracer::IdentifyBitMode(const char *bin_path) {
    /* open binary file */
    ifstream bin_file(bin_path);
    if ( bin_file.is_open() == false ) {
        perror("Tracer::IdentifyBitMode() failed");
        cout << " --> bin_file.is_open() == false" << endl;
        return FAILED_TO_IDENTIFY_BIT_MODE;
    }
    /* read one byte from elf header that means 32 or 64 */
    bin_file.seekg(4, ios::beg);
    char mode;
    bin_file.read(&mode, 1);
    if ( mode == 1 ) {
        bit_mode_ = MODE_I386;
    } else if ( mode == 2 ) {
        bit_mode_ = MODE_X86_64;
    } else {
        bin_file.close();
        perror("Tracer::IdentifyBitMode() failed");
        cout << " --> can't identify bit mode " << endl;
        return FAILED_TO_IDENTIFY_BIT_MODE;
    }
    return SUCCESS_TO_IDENTIFY_BIT_MODE;
}

int Tracer::StartLoggingRIP() {
    /* create log file that its name is tracee's path.rip */
    log_file_.open(log_path_.c_str(), ios::out | ios::binary);
    if ( log_file_.is_open() == false ) {
        perror("Tracer::StartLoggingRIP() failed");
        cout << "  --> log_file_.is_open() == false" << endl;
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
        perror("Tracer::StepInto() failed");
        cout << "  --> WIFSIGNALED(status) || WIFEXITED(status)" << endl;
        return FAILED_TO_STEP_INTO;
    }
    /* check if the tracee is crashed */
    siginfo_t siginfo;
    if ( ptrace(PTRACE_GETSIGINFO, child_pid_, NULL, &siginfo) == -1 ) {
        perror("Tracer::StepInto() failed");
        cout << "  --> ptrace(PTRACE_GETSIGINFO, child_pid_, NULL, &siginfo) == -1 " << endl;
        return FAILED_TO_STEP_INTO;
    }
    int signo = siginfo.si_signo;
    if ( signo == SIGILL || signo == SIGSEGV || signo == SIGFPE || signo == SIGCHLD ) {
        perror("Tracer::StepInto() failed");
        cout << "  --> signo == SIGILL || signo == SIGSEGV || signo == SIGFPE || signo"\
                " == SIGCHLD" << endl;
        return FAILED_TO_STEP_INTO;
    }
    /* handle before step into like code hooking */
    if ( HandleDuringStepInto() == FAILED_TO_HANDLE_DURING_STEP_INTO ) {
        perror("Tracer::StepInto() failed");
        cout << "  --> HandleDuringStepInto() == FAILED_TO_HANDLE_DURING_STEP_INTO" << endl;
        return FAILED_TO_STEP_INTO;
    }
    /* step into */
    if ( ptrace(PTRACE_SINGLESTEP, child_pid_, NULL, NULL) == -1 ) {
        perror("Tracer::StepInto() failed");
        cout << "  --> ptrace(PTRACE_SINGLESTEP, child_pid_, NULL, NULL) == -1" << endl;
        return FAILED_TO_STEP_INTO;
    }
    return SUCCESS_TO_STEP_INTO;
}

int Tracer::HandleDuringStepInto() {
    /* get current registers and log rip */
    struct user_regs_struct regs;
    if ( ptrace(PTRACE_GETREGS, child_pid_, NULL, &regs) == -1 ) {
        perror("Tracer::HandleDuringStepInto() failed");
        cout << "  --> ptrace(PTRACE_GETREGS, child_pid_, NULL, &regs) == -1" << endl;
        return FAILED_TO_HANDLE_DURING_STEP_INTO;
    }
    LogRIP(regs.rip);
    /* append rip to it's rip tracing log */
    return SUCCESS_TO_HANDLE_DURING_STEP_INTO;
}

