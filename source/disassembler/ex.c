#include <inttypes.h>

#include <capstone/capstone.h>


int main(void) {
    char *code = "I\x89\xd1^H\x89\xe2H";
    csh handle;
    cs_insn *insn;
    size_t count;

    if ( cs_open(CS_ARCH_X86, CS_MODE_64, &handle) != CS_ERR_OK ) {
        return -1;
    }

    count = cs_disasm(handle, code, 7, 0x1000, 0, &insn);
    if ( count > 0 ) {
        size_t j;
        for ( j = 0; j < count; j++ ) {
            printf("0x%"PRIx64":\t%s\t\t%s\n", insn[j].address, insn[j].mnemonic, insn[j].op_str);
        }
        cs_free(insn, count);

    } else {
        printf("ERROR: Failed to disassemble given code!\n");
    }

    cs_close(&handle);

    return 0;
}

