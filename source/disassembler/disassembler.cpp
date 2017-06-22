/* ./disassembler/disassembler.cpp
 * by kimht
 */
///////////////////////////////////////////////////////////////////////////////
/// include
///////////////////////////////////////////////////////////////////////////////
#include "./disassembler.h"

Disassembler *Disassembler::instance = 0;

///////////////////////////////////////////////////////////////////////////////
/// public
///////////////////////////////////////////////////////////////////////////////

/*
 * GetInstance - For singleton.
 */
Disassembler *Disassembler::GetInstance() {
    if ( !instance ) {
        instance = new Disassembler();
    }
    return instance;
}

/*
 * OpenCS   - Open Capstone handle to prepare to disassemble machine code.
 *
 * @return  - true on success; false on failure.
 */
bool Disassembler::OpenCS(int arch) {
    if ( cs_open(CS_ARCH_X86, (arch == ARCH_X86_64) ? CS_MODE_64 : CS_MODE_32, &cs_handle_) != CS_ERR_OK ) {
       HANDLE_ERROR("Disassembler::OpenCS() failure", false); 
    }

    return true;
}

/*
 * DisassembleCode  - Disassembles given code and return the result of decoding.
 *
 * @code            - The address of machine code there to be disassembled.
 * @out             - The pointer that takes the result of decoding.
 * @return          - true on success; false on failure.
 */
bool Disassembler::DisassembleCode(const unsigned char *code, uint64_t pc, string &out_mnemonic, string &out_op_str) {
    cs_insn *insn;

    // decode given code to string.
    size_t count = cs_disasm(cs_handle_, code, 15, pc, 0, &insn);
    if ( count > 0 ) {
        out_mnemonic = (string)insn[0].mnemonic;
        out_op_str = (string)insn[0].op_str;
        
        cs_free(insn, count);

        return true;
    }

    return false;
}

/*
 * CloseCS - Close Capstone handle opened.
 */
void Disassembler::CloseCS() {
    cs_close(&cs_handle_);
}

///////////////////////////////////////////////////////////////////////////////
/// private
///////////////////////////////////////////////////////////////////////////////

Disassembler::Disassembler() {

    // Init branch instructions. It's generated automatically.
    branch_insn_.insert("ret");
    branch_insn_.insert("jmp");
    branch_insn_.insert("ja");
    branch_insn_.insert("jae");
    branch_insn_.insert("jb");
    branch_insn_.insert("jbe");
    branch_insn_.insert("jc");
    branch_insn_.insert("jcxz");
    branch_insn_.insert("jecxz");
    branch_insn_.insert("je");
    branch_insn_.insert("jg");
    branch_insn_.insert("jge");
    branch_insn_.insert("jl");
    branch_insn_.insert("jle");
    branch_insn_.insert("jna");
    branch_insn_.insert("jnae");
    branch_insn_.insert("jnb");
    branch_insn_.insert("jnbe");
    branch_insn_.insert("jnc");
    branch_insn_.insert("jne");
    branch_insn_.insert("jng");
    branch_insn_.insert("jnge");
    branch_insn_.insert("jnl");
    branch_insn_.insert("jnle");
    branch_insn_.insert("jno");
    branch_insn_.insert("jnp");
    branch_insn_.insert("jns");
    branch_insn_.insert("jnz");
    branch_insn_.insert("jo");
    branch_insn_.insert("jp");
    branch_insn_.insert("jpe");
    branch_insn_.insert("jpo");
    branch_insn_.insert("js");
    branch_insn_.insert("jz");
    branch_insn_.insert("ja");
    branch_insn_.insert("jae");
    branch_insn_.insert("jb");
    branch_insn_.insert("jbe");
    branch_insn_.insert("jc");
    branch_insn_.insert("je");
    branch_insn_.insert("jz");
    branch_insn_.insert("jg");
    branch_insn_.insert("jge");
    branch_insn_.insert("jl");
    branch_insn_.insert("jle");
    branch_insn_.insert("jna");
    branch_insn_.insert("jnae");
    branch_insn_.insert("jnb");
    branch_insn_.insert("jnbe");
    branch_insn_.insert("jnc");
    branch_insn_.insert("jne");
    branch_insn_.insert("jng");
    branch_insn_.insert("jnge");
    branch_insn_.insert("jnl");
    branch_insn_.insert("jnle");
    branch_insn_.insert("jno");
    branch_insn_.insert("jnp");
    branch_insn_.insert("jns");
    branch_insn_.insert("jnz");
    branch_insn_.insert("jo");
    branch_insn_.insert("jp");
    branch_insn_.insert("jpe");
    branch_insn_.insert("jpo");
    branch_insn_.insert("js");
    branch_insn_.insert("jz");
}

/*
 *
 *
 *
 */
bool Disassembler::IsBranchInstruction(string mnemonic) {
    set<string>::iterator found = branch_insn_.find(mnemonic);
    if ( found == branch_insn_.end() ) {
        return false;
    }

    return true;
}

