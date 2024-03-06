#include "cpu/instr.h"

static void instr_execute_1op()
{
    operand_read(&opr_src);
    opr_dest.type = OPR_MEM;
    opr_dest.sreg = SREG_SS;
    if(opr_src.data_size == 8)
    {
        opr_src.val = sign_ext(opr_src.val, 8);
    }
    opr_dest.data_size = opr_src.data_size = 32;
    opr_dest.addr = cpu.esp - data_size / 8; 
    opr_dest.mem_addr.disp = opr_dest.addr; 
    
    opr_dest.val = opr_src.val;
    operand_write(&opr_dest);
    
    cpu.esp -= opr_src.data_size / 8;
    
    return ;
}

make_instr_impl_1op(push, rm, v)
make_instr_impl_1op(push, r, v)
make_instr_impl_1op(push, i, b)
make_instr_impl_1op(push, i, v)

static void push_(uint32_t x)
{
    opr_dest.type = OPR_MEM;
    opr_dest.sreg = SREG_SS;
    
    opr_dest.data_size = 32;
    opr_dest.addr = cpu.esp - 4; 
    //opr_dest.mem_addr.disp = opr_dest.addr; 
    
    opr_dest.val = x;
    operand_write(&opr_dest);
    
    cpu.esp -= 4;
}
make_instr_func(pusha)
{
    //assert(0);
    int len = 1;
    uint32_t oldesp = cpu.esp;
    push_(cpu.eax);
    push_(cpu.ecx);
    push_(cpu.edx);
    push_(cpu.ebx);
    push_(oldesp);
    push_(cpu.ebp);
    push_(cpu.esi);
    push_(cpu.edi);
    return len;
}
/*
Put the implementations of `push' instructions here.
*/
