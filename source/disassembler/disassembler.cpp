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
 * @output          - The pointer that takes the result of decoding.
 * @return          - true on success; false on failure.
 */
bool Disassembler::DisassembleCode(const unsigned char *code, string& output) {
    cs_insn *insn;

    // decode given code to string.
    size_t count = cs_disasm(cs_handle_, code, 15, 0x0, 0, &insn);
    if ( count > 0 ) {
        output = (string)insn[0].mnemonic + "\t" + (string)insn[0].op_str;
        
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
}

