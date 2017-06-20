/* ./disassembler/disassembler.h
 * by kimht
 */
#ifndef _DISASSEMBLER_DISASSEMBLER_H_
#define _DISASSEMBLER_DISASSEMBLER_H_

//////////////
/// include
//////////////
#include "../base/base.h"

/////////////
/// define
/////////////

////////////
/// class
////////////
class Tracer {
public:
    static Tracer *GetInstance();
    int StartTracingProgram(const char *path, char *const argv[], char *const envp[]);

private:
    static Tracer *instance;
    string program_path_;
    pid_t child_pid_;
    int bit_mode_;

    Tracer();
    int IdentifyBitMode(const char *bin_path);
    int RepeatSingleStep();
    int HandlerSingleStep();
};

#endif

