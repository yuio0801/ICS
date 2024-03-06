#include "cpu/instr.h"

make_instr_func(leave)
{
    cpu.esp = cpu.ebp;
    
    opr_src.type = OPR_MEM;
    opr_src.sreg = SREG_SS;
    opr_src.data_size = data_size;
    opr_src.addr = cpu.esp; /*???*/
    opr_src.mem_addr.disp = opr_src.addr; /*???*/
    operand_read(&opr_src);
    
    cpu.ebp = opr_src.val;
    
    cpu.esp += opr_src.data_size / 8;
    
    return 1;
}

/*
Put the implementations of `leave' instructions here.
*/
