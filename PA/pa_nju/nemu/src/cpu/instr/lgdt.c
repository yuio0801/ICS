#include "cpu/instr.h"
/*
Put the implementations of `lgdt' instructions here.
*/
GDTR m1632;

make_instr_func(lgdt)
{
    int len = 1;
    OPERAND addr;
    len += modrm_rm(eip + 1, &addr);
    
    m1632.limit = laddr_read(addr.addr, 2);
    m1632.base = laddr_read(addr.addr + 2, 4);
    
    
    //printf("base:%x %x",m1632.base, m1632.limit);
    cpu.gdtr = m1632;
    
    print_asm_1("lgdt", "", 6, &addr);
    
    return len;
}

