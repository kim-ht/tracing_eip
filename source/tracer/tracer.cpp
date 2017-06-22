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

        child_pid_ = pid;

        // Do single step.
        i = ptrace(PTRACE_SINGLESTEP, child_pid_, 0, 0);
        if ( i < 0 ) {
            HANDLE_ERROR("Tracer::StartTracingProgram() failure", false);
        }

        // Starts logging/tracing program counter.
        b = Logger::GetInstance()->OpenLogFile(((string)path + ".pc").c_str(), 4 + 4*arch_);
        if ( !b ) {
            HANDLE_ERROR("Tracer::StartTracingProgram() failure", false);
        }

        // Starts disassembler.
        b = Disassembler::GetInstance()->OpenCS(arch_);
        if ( !b ) {
            HANDLE_ERROR("Tracer::StartTracingProgram() failure", false);
        }

        // Single step loop.
        RepeatSingleStep(); 

        // End up logging.
        Logger::GetInstance()->CloseLogFile();
        Disassembler::GetInstance()->CloseCS();
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
    ret_addr_.empty();
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
    uint64_t pc = regs.rip;

    // Record the line.
    if ( (pc & 0x7f0000000000) != 0x7f0000000000 ) {

        // Pop if current pc is an address that pushed to ret stack.
        if ( !ret_addr_.empty() && pc == ret_addr_.back() ) {
            PopRetAddr();
        }

        // Gets code from child process and disassemble it.
        unsigned char *code = 0; // It should be free'ed after using it.
        GetDataFromPID(child_pid_, (uint64_t)pc, 15, &code);

        // Gets mnemonic and operand.
        string mnemonic, op_str;
        Disassembler::GetInstance()->DisassembleCode(code, pc, mnemonic, op_str);
        free(code);

        // Organizes line.
        Line line;
        line.pc = pc;
        line.mnemonic = mnemonic;
        line.op_str = op_str;
        line.is_branch = Disassembler::GetInstance()->IsBranchInstruction(mnemonic);
        line.level = ret_addr_.size();

        // Records.
        Logger::GetInstance()->RecordLine(line);

        // Pushes return address(pc + 5) if currenct instruction is call.
        if ( line.mnemonic == "call" ) {
            PushRetAddr(pc + 5);
        }
    }

    return true;
}

/*
 * GetDataFromPID - Gets data from a process specificed by PID.
 *
 * @pid - The PID to be read.
 * @addr - The address to be read from.
 * @size - The size to read.
 * @output - pointer to write read data.
 */
bool Tracer::GetDataFromPID(long pid, uint64_t addr, size_t size, unsigned char **output) {
    string path = "/proc/" + to_string(pid) + "/mem";

    int fd = open(path.c_str(), O_RDONLY);

    *output = (unsigned char *)malloc(size);
    if ( !(*output) ) {
        HANDLE_ERROR("Tracer::GetDataFromPID failure", false);
    }

    int i = pread(fd, *output, size, addr);
    if ( i < 0 ) {
        HANDLE_ERROR("Tracer::GetDataFromPID failure", false);
    }

    close(fd);

    return true;
}

/*
 *
 */
void Tracer::PushRetAddr(uint64_t addr) {
    ret_addr_.push_back(addr);
}

/*
 *
 */
uint64_t Tracer::PopRetAddr() {
    uint64_t tmp = ret_addr_.back();
    ret_addr_.pop_back();
    return tmp;
}

