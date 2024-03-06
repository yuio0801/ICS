#include "cpu/instr.h"
/*
Put the implementations of `iret' instructions here.
*/

static void pop_(uint32_t *x)
{
    opr_dest.type = OPR_MEM;
    opr_dest.sreg = SREG_SS;
    opr_dest.data_size = 32;
    opr_dest.addr = cpu.esp; 
    //opr_dest.mem_addr.disp = opr_dest.addr; 
    operand_read(&opr_dest);
    *x = opr_dest.val;
    
    cpu.esp += 4;
}

make_instr_func(iret)
{
    int len = 0;
    pop_(&cpu.eip);
    uint32_t val;
    pop_(&val);
    cpu.segReg[SREG_CS].val = val;
    load_sreg(SREG_CS);
    pop_(&cpu.eflags.val);
    
    return len;
}