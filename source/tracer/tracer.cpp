/* ./tracer/tracer.cpp
 * by kimht
 */
#include "./tracer.h"

Tracer *Tracer::instance = 0;

/////////////
/// public
/////////////

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
 * @return              - SUCCESS_TO_START_TRACE_PROGRAM(0) on success; FAILED_TO_START_TRACE_PROGRAM(-1) on failure
 */
int Tracer::StartTracingProgram(const char *path, char *const argv[], char *const envp[]) {

    // identify 32bit or 64bit binary
    if ( IdentifyBitMode(path) == FAILED_TO_IDENTIFY_BIT_MODE ) {
        perror("Tracer::StartTracingProgram() failed");
        return FAILED_TO_START_TRACE_PROGRAM;
    }

    pid_t pid = fork();

    /* if fork() is failed */
    if ( pid < 0 ) {
        perror("Tracer::StartTracingProgram() failed");
        return FAILED_TO_START_TRACE_PROGRAM;

    /* if pid is child's pid */
    } else if ( pid == 0 ) {
        if ( ptrace(PTRACE_TRACEME, 0, 0, 0) < 0 ) {
            perror("Tracer::StartTracingProgram() failed");
            return FAILED_TO_START_TRACE_PROGRAM;
        }
        if ( execvpe(path, argv, envp) < 0 ) {
            perror("Tracer::StartTracingProgram() failed");
            return FAILED_TO_START_TRACE_PROGRAM;
        }

    /* if pid is parent's pid */
    } else {
        if ( wait(0) < 0 ) {
            perror("Tracer::StartTracingProgram() failed");
            return FAILED_TO_START_TRACE_PROGRAM;
        }
        child_pid_ = pid;
        if ( ptrace(PTRACE_SINGLESTEP, child_pid_, 0, 0) < 0 ) {
            perror("Tracer::StartTracingProgram() failed");
            return FAILED_TO_START_TRACE_PROGRAM; 
        }

        // start logging/tracing rip
        if ( Logger::GetInstance()->OpenLogFile(((string)path + ".pc").c_str(), 4 + 4*bit_mode_) == FAILED_TO_OPEN_LOG_FILE ) {
            perror("Tracer::StartTracingProgram() failed");
            return FAILED_TO_START_TRACE_PROGRAM;
        }

        // single step loop
        RepeatSingleStep();
        Logger::GetInstance()->CloseLogFile();
    }
    return SUCCESS_TO_START_TRACE_PROGRAM;
}

//////////////
/// private
//////////////

/*
 * Tracer - The constructor.
 */
Tracer::Tracer() {
}

/*
 * IdentifyBitMode  - Identifies arhctecture(32 or 64) that given elf requires.
 *
 * @bin_path        - The path of elf binary file.
 * @return          - SUCCESS_TO_IDENTIFY_BIT_MODE(0) on success; FAILED_TO_IDENTIFY_BIT_MODE(-1) on failure
 */
int Tracer::IdentifyBitMode(const char *bin_path) {

    // open binary file
    ifstream bin_file(bin_path);
    if ( bin_file.is_open() == false ) {
        perror("Tracer::IdentifyBitMode() failed");
        cout << " --> bin_file.is_open() == false" << endl;
        return FAILED_TO_IDENTIFY_BIT_MODE;
    }

    // read one byte from elf header that means 32 or 64
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

/*
 * RepeatSingleStep - repeats single step and handles it.
 *
 * @return          - SUCCESS_TO_STEP_INTO
 */
int Tracer::RepeatSingleStep() {
    int status;
    if ( waitpid(child_pid_, &status, 0) < 0 ) {
        perror("Tracer::RepeatSingleStep() failed");
        return FAILED_TO_REPEAT_SINGLE_STEP;
    }

    while ( WIFSTOPPED(status) ) {

        // check if the tracee is crashed
        siginfo_t siginfo;
        if ( ptrace(PTRACE_GETSIGINFO, child_pid_, 0, &siginfo) < 0 ) {
            perror("Tracer::RepeatSingleStep() failed");
            return FAILED_TO_REPEAT_SINGLE_STEP;
        }
        int signo = siginfo.si_signo;
        if ( signo == SIGILL || signo == SIGSEGV || signo == SIGFPE || signo == SIGCHLD ) {
            perror("Tracer::RepeatSingleStep() failed");
            return FAILED_TO_REPEAT_SINGLE_STEP;
        }

        // handle before step into like code hooking
        if ( HandlerSingleStep() == FAILED_TO_HANDER_SINGLE_STEP ) {
            perror("Tracer::RepeatSingleStep() failed");
            return FAILED_TO_REPEAT_SINGLE_STEP;
        }

        // single step
        if ( ptrace(PTRACE_SINGLESTEP, child_pid_, 0, 0) < 0 ) {
            perror("Tracer::RepeatSingleStep() failed");
            return FAILED_TO_REPEAT_SINGLE_STEP;
        }

        // waitpid
        if ( waitpid(child_pid_, &status, 0) < 0 ) {
            perror("Tracer::RepeatSingleStep() failed");
            return FAILED_TO_REPEAT_SINGLE_STEP;
        }
    }
    return SUCCESS_TO_HANDER_SINGLE_STEP;
}

/*
 * HanderSingleStep - The hander that occurs just before single step.
 *
 * @return          - SUCCESS_TO_HANDER_SINGLE_STEP(0) on success; FAILED_TO_HANDER_SINGLE_STEP(-1) on failure;
 */
int Tracer::HandlerSingleStep() {

    // get current registers
    struct user_regs_struct regs;
    if ( ptrace(PTRACE_GETREGS, child_pid_, 0, &regs) < 0 ) {
        perror("Tracer::HandlerSingleStep() failed");
        return FAILED_TO_HANDER_SINGLE_STEP;
    }

    // record the rip
    Logger::GetInstance()->RecordPC(regs.rip);
    return SUCCESS_TO_HANDER_SINGLE_STEP;
}

