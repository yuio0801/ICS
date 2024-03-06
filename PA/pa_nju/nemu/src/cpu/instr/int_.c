#include "cpu/intr.h"
#include "cpu/instr.h"

/*
Put the implementations of `int' instructions here.

Special note for `int': please use the instruction name `int_' instead of `int'.
*/

make_instr_func(int_)
{
    //assert(0);
    
    OPERAND r;
    r.data_size = 8;
    r.type = OPR_IMM;
    r.sreg = SREG_CS;
    r.addr = eip + 1;
    operand_read(&r);
    
    raise_sw_intr(r.val);
    
    print_asm_1("int", "", 2, &r);
    return 0;
}