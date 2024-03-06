#include "cpu/instr.h"

make_instr_func(lea){
    int len = 1;
	OPERAND r, m;
	r.data_size = data_size;

	len += modrm_r_rm(eip + 1, &r, &m);
	
	r.val = m.addr;
	operand_write(&r);

	print_asm_2("lea", "", len, &m, &r);
	return len;
    
}

/*
Put the implementations of `lea' instructions here.
*/
