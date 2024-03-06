#include "cpu/instr.h"
/*
Put the implementations of `lidt' instructions here.
*/
IDTR m1632;

make_instr_func(lidt)
{
    int len = 1;
    OPERAND addr;
    len += modrm_rm(eip + 1, &addr);
    
    m1632.limit = laddr_read(addr.addr, 2);
    m1632.base = laddr_read(addr.addr + 2, 4);
    
    
    cpu.idtr = m1632;
    
    print_asm_1("lidt", "", 6, &addr);
    
    return len;
}

