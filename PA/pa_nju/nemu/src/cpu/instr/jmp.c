#include "cpu/instr.h"

make_instr_func(jmp_near)
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
}
make_instr_func(jmp_short)
{
    OPERAND rel;
    rel.type = OPR_IMM;
    rel.sreg = SREG_CS;
    rel.data_size = 8;
    rel.addr = eip + 1;

    operand_read(&rel);

    int offset = sign_ext(rel.val, 8);

    print_asm_1("jmp", "", 2, &rel);

    cpu.eip += offset;

    return 2;
}

make_instr_func(jmp_near_indirect)
{
    int len = 1;
    OPERAND rm;
    rm.data_size = data_size;
    len += modrm_rm(eip + 1, &rm);
    operand_read(&rm);
    cpu.eip = rm.val;
    
    return 0;
}

make_instr_func(jmp_far_imm)
{
    int len = 1;
    len += 6;
    OPERAND ptr1, ptr2;
    ptr1.type = ptr2.type = OPR_IMM;
    ptr1.sreg = ptr2.sreg = SREG_CS;
    ptr1.data_size = 16;
    ptr2.data_size = 32;
    ptr1.addr = eip + 5;
    ptr2.addr = eip + 1;
    operand_read(&ptr1);
    operand_read(&ptr2);
    cpu.cs.val = (uint16_t)ptr1.val;
    print_asm_1("ljmp", "", 7, &ptr2);
    cpu.eip = ptr2.val;
    
    //printf("ptr:%x %x\n",ptr1.val, ptr2.val);
    load_sreg(SREG_CS);
    
    //printf("val:eip: %x %x \n",cpu.cs.val, cpu.eip);
    return 0;
}