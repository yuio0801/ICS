#include "cpu/instr.h"

make_instr_func(ret_near)
{
    opr_src.type = OPR_MEM;
    opr_src.sreg = SREG_SS;
    opr_src.data_size = data_size;
    opr_src.addr = cpu.esp; 
    //opr_src.mem_addr.disp = opr_src.addr; //???
    operand_read(&opr_src);
    
    cpu.eip = opr_src.val;
    if(data_size == 16)
    {
        cpu.eip = cpu.eip & 0xFFFF;
    }
    
    
    cpu.esp += opr_src.data_size / 8;
    
    return 0;
}

make_instr_func(ret_near_imm16)
{
    cpu.eip = instr_fetch(eip + 1, 2);
    
    opr_src.type = OPR_MEM;
    opr_src.sreg = SREG_SS;
    opr_src.data_size = data_size;
    opr_src.addr = cpu.esp; //???
    opr_src.mem_addr.disp = opr_src.addr; //???
    operand_read(&opr_src);
    
    cpu.eip = opr_src.val;
    if(data_size == 16)
    {
        cpu.eip = cpu.eip & 0xFFFF;
    }
    
    cpu.esp += opr_src.data_size / 8;
    
    return 0;
}


/*
Put the implementations of `ret' instructions here.
*/
