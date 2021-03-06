/* ./disassembler/disassembler.h
 * by kimht
 */
#ifndef _DISASSEMBLER_DISASSEMBLER_H_
#define _DISASSEMBLER_DISASSEMBLER_H_

///////////////////////////////////////////////////////////////////////////////
/// include
///////////////////////////////////////////////////////////////////////////////
#include "../base/base.h"

///////////////////////////////////////////////////////////////////////////////
/// define
///////////////////////////////////////////////////////////////////////////////
#define ARCH_I386       0
#define ARCH_X86_64     1

///////////////////////////////////////////////////////////////////////////////
/// class
///////////////////////////////////////////////////////////////////////////////
class Disassembler {
public:
    static Disassembler *GetInstance();
    bool OpenCS(int arch);
    bool DisassembleCode(const unsigned char *code, uint64_t pc, string &out_mnemonic, string &out_op_str);
    void CloseCS();
    bool IsBranchInstruction(string mnemonic);

private:
    static Disassembler *instance;
    csh cs_handle_;
    set<string> branch_insn_;

    Disassembler();
};



#endif

