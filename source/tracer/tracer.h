/* ./tracer/tracer.h
 * by kimht
 */
#ifndef _TRACER_TRACER_H_
#define _TRACER_TRACER_H_

///////////////////////////////////////////////////////////////////////////////
/// include
///////////////////////////////////////////////////////////////////////////////
#include "../base/base.h"
#include "../logger/logger.h"
#include "../disassembler/disassembler.h"

///////////////////////////////////////////////////////////////////////////////
/// define
///////////////////////////////////////////////////////////////////////////////
#define ARCH_I386       0
#define ARCH_X86_64     1

///////////////////////////////////////////////////////////////////////////////
/// class
///////////////////////////////////////////////////////////////////////////////
class Tracer {
public:
    static Tracer *GetInstance();
    bool StartTracingProgram(const char *path, char *const argv[], char *const envp[]);

private:
    static Tracer *instance;
    string program_path_;
    pid_t child_pid_;
    int arch_;

    Tracer();
    bool IdentifyArch(const char *bin_path);
    bool RepeatSingleStep();
    bool HandlerSingleStep();
    bool GetDataFromPID(long pid, uint64_t addr, size_t size, unsigned char **output);
    bool FreeCode();
};

#endif

