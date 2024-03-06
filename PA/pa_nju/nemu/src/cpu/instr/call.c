#include "cpu/instr.h"
make_instr_func(call_near) {
    
	OPERAND rel;
	rel.type = OPR_IMM;
	rel.sreg = SREG_CS;
	rel.data_size = data_size;
	rel.addr = eip + 1;
	
	operand_read(&rel);
	
	//push()
    opr_dest.type = OPR_MEM;
    opr_dest.sreg = SREG_SS;
    opr_dest.data_size = data_size;
    opr_dest.addr = cpu.esp - data_size / 8; 
    //opr_dest.mem_addr.disp = opr_dest.addr;
    
    opr_dest.val = eip + 1 + data_size / 8;
    operand_write(&opr_dest);
    
    cpu.esp -= data_size / 8;
	
	cpu.eip += rel.val;
// 	if(data_size == 16)
// 	{
// 	    cpu.eip = cpu.eip & 0xffff;
// 	}

	print_asm_1("call_near", "", 0, &rel);
	return 1 + data_size / 8;
}
make_instr_func(call_near_indirect){
    int len = 1;
    OPERAND rm;
    rm.data_size = data_size;
    len += modrm_rm(eip + 1, &rm);
    
    //push()
    opr_dest.type = OPR_MEM;
    opr_dest.sreg = SREG_SS;
    opr_dest.data_size = data_size;
    opr_dest.addr = cpu.esp - data_size / 8; 
    //opr_dest.mem_addr.disp = opr_dest.addr;
    
    opr_dest.val = eip + len;
    operand_write(&opr_dest);
    
    cpu.esp -= data_size / 8;
	
	operand_read(&rm);
    cpu.eip = rm.val;
// 	if(data_size == 16)
// 	{
// 	    cpu.eip = cpu.eip & 0xffff;
// 	}
    
    return 0;
    
    
}
/*
Put the implementations of `call' instructions here.
*/
/*make_instr_func(jmp_near)
{
        OPERAND rel;
        rel.type = OPR_IMM;
        rel.sreg = SREG_CS;
        rel.data_size = data_size;
        rel.addr = eip + 1;

        operand_read(&rel);

        int offset = sign_ext(rel.val, data_size);
        // thank Ting Xu from CS'17 for finding this bug
        print_asm_1("jmp", "", 1 + data_size / 8, &rel);

        cpu.eip += offset;

        return 1 + data_size / 8;
}*/