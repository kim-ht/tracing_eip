/* ./tracer/tracer.cpp
 * by kimht
 */
///////////////////////////////////////////////////////////////////////////////
/// include
///////////////////////////////////////////////////////////////////////////////
#include "./tracer.h"

Tracer *Tracer::instance = 0;

///////////////////////////////////////////////////////////////////////////////
/// public
///////////////////////////////////////////////////////////////////////////////

/*
 * GetInstance - For singleton.
 */
Tracer *Tracer::GetInstance() {
    if ( !instance ) {
        instance = new Tracer();
    }
    return instance;
}

/*
 * StartTracingProgram  - Start tracing a program. child. 
 *
 * @path                - The path of the program to be traced.
 * @argv                - The argv which will be delivered to tracee program.
 * @envp                - The envp which will be delivered to tracee program.
 * @return              - true on success; false on failure.
 */
bool Tracer::StartTracingProgram(const char *path, char *const argv[], char *const envp[]) {

    // Identify the binary is 32bit or 64bit.
    bool b = IdentifyArch(path);
    if ( !b ) {
        HANDLE_ERROR("Tracer::StartTracingProgram() failure", false);
    }

    pid_t pid = fork();

    /* Negative PID means fork() failure. */
    if ( pid < 0 ) {
        HANDLE_ERROR("Tracer::StartTracingProgram() failure", false);

    /*
     * Zero PID means child process. It starts tracing the child process with
     * ptrace for tracing/recording program counter.
     */
    } else if ( pid == 0 ) {
        int i = ptrace(PTRACE_TRACEME, 0, 0, 0);
        if ( i < 0 ) {
            HANDLE_ERROR("Tracer::StartTracingProgram() failure", false);
        }

        i = execvpe(path, argv, envp);
        if ( i < 0 ) {
            HANDLE_ERROR("Tracer::StartTracingProgram() failure", false);
        }
    /*
     * Positive PID means parant process. it waits the child process and then
     * repeats single step logging the program counter.
     */
    } else {
        int i = wait(0);
        if ( i < 0 ) {
            HANDLE_ERROR("Tracer::StartTracingProgram() failure", false);
        }

        // Do single step.
        child_pid_ = pid;
        i = ptrace(PTRACE_SINGLESTEP, child_pid_, 0, 0);
        if ( i < 0 ) {
            HANDLE_ERROR("Tracer::RepeatSingleStep() failure", false);
        }

        // Starts logging/tracing program counter.
        b = Logger::GetInstance()->OpenLogFile(((string)path + ".pc").c_str(), 4 + 4*arch_);
        if ( !b ) {
            HANDLE_ERROR("Tracer::StartTracingProgram() failure", false);
        }

        // Single step loop.
        RepeatSingleStep(); 

        // End up logging.
        Logger::GetInstance()->CloseLogFile();
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
/// private
///////////////////////////////////////////////////////////////////////////////

/*
 * Tracer - The constructor.
 */
Tracer::Tracer() {
}

/*
 * IdentifyArch - Identifies arhctecture(i386 or x86-64) that given ELF binary
 *                requires.
 *
 * @bin_path    - The path of ELF binary file.
 * @return      - true on success; false on failure.
 */
bool Tracer::IdentifyArch(const char *bin_path) {

    // Open binary file.
    ifstream bin_file(bin_path);
    if ( !bin_file.is_open() ) {
        HANDLE_ERROR("Tracer::IdentifyArch() failure", false);
    }

    // Read one byte from ELF header meaning archtecture.
    bin_file.seekg(4, ios::beg);
    char arch;
    bin_file.read(&arch, 1);

    // Determines arch_ is i386 or x86-64.
    if ( arch == 1 ) {
        arch_ = ARCH_I386;
    } else if ( arch == 2 ) {
        arch_ = ARCH_X86_64;
    } else {
        bin_file.close();
        HANDLE_ERROR("Tracer::IdentifyArch() failure", false);
    }

    return true;
}

/*
 * RepeatSingleStep - repeats single step and handles it.
 *
 * @return          - true on success; false on failure.
*/
bool Tracer::RepeatSingleStep() {

    // Waits until the tracee end up execution an instruction.
    int status;
    int i = waitpid(child_pid_, &status, 0);
    if ( i < 0 ) {
        HANDLE_ERROR("Tracer::RepeatSingleStep() failure", false);
    }

    while ( WIFSTOPPED(status) ) {

        // Checks if the tracee is crashed.
        siginfo_t siginfo;
        i = ptrace(PTRACE_GETSIGINFO, child_pid_, 0, &siginfo);
        if ( i < 0 ) {
            HANDLE_ERROR("Tracer::RepeatSingleStep() failure", false);
        }
        int signo = siginfo.si_signo;
        if ( signo == SIGILL || signo == SIGSEGV || signo == SIGFPE || signo == SIGCHLD ) {
            return true;
        }

        // Handles after single step like code hooking.
        bool b = HandlerSingleStep();
        if ( !b ) {
            HANDLE_ERROR("Tracer::RepeatSingleStep() failure", false);
        }

        // Do single step.
        i = ptrace(PTRACE_SINGLESTEP, child_pid_, 0, 0);
        if ( i < 0 ) {
            HANDLE_ERROR("Tracer::RepeatSingleStep() failure", false);
        }

        // Waits until the tracee end up execution an instruction.
        i = waitpid(child_pid_, &status, 0);
        if ( i < 0 ) {
            HANDLE_ERROR("Tracer::RepeatSingleStep() failure", false);
        }
    }

    return true;
}

/*
 * HanderSingleStep - The hander that occurs just after single step.
 *
 * @return - true on success; false on failure.
 */
bool Tracer::HandlerSingleStep() {

    // Get current registers values.
    struct user_regs_struct regs;
    int i = ptrace(PTRACE_GETREGS, child_pid_, 0, &regs);
    if ( i < 0 ) {
        HANDLE_ERROR("Tracer::HandlerSingleStep()", false);
    }

    // Record the program counter.
    Logger::GetInstance()->RecordPC(regs.rip);

    return true;
}

