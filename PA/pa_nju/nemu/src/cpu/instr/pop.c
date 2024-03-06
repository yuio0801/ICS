#include "cpu/instr.h"

static void instr_execute_1op()
{
    operand_read(&opr_src);
    opr_dest.type = OPR_MEM;
    opr_dest.sreg = SREG_SS;
    opr_dest.data_size = data_size;
    opr_dest.addr = cpu.esp; /*???*/
    opr_dest.mem_addr.disp = opr_dest.addr; /*???*/
    operand_read(&opr_dest);
    opr_src.val = opr_dest.val;
    operand_write(&opr_src);
    
    cpu.esp += data_size / 8;
    
    return ;
}

make_instr_impl_1op(pop, r, v)

static void pop_(uint32_t *x)
{
    opr_dest.type = OPR_MEM;
    opr_dest.sreg = SREG_SS;
    opr_dest.data_size = 32;
    opr_dest.addr = cpu.esp; /*???*/
    //opr_dest.mem_addr.disp = opr_dest.addr; /*???*/
    operand_read(&opr_dest);
    *x = opr_dest.val;
    
    cpu.esp += 4;
}
make_instr_func(popa)
{
    int len = 1;
    uint32_t oldesp = cpu.esp;
    pop_(&cpu.edi);
    pop_(&cpu.esi);
    pop_(&cpu.ebp);
    pop_(&oldesp);
    pop_(&cpu.ebx);
    pop_(&cpu.edx);
    pop_(&cpu.ecx);
    pop_(&cpu.eax);
    return len;
}

/*
Put the implementations of `pop' instructions here.
*/
