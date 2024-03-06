#include "cpu/instr.h"

make_instr_func(mul_rm2a_b)
{
        int len = 1; // in group
        OPERAND al, rm;
        al.data_size = 8;
        al.type = OPR_REG;
        al.addr = REG_AL;
        rm.data_size = 8;
        len += modrm_rm(eip + 1, &rm);
        operand_read(&al);
        operand_read(&rm);
        // zero extent
        uint32_t res = alu_mul(rm.val, al.val, 8);
        OPERAND ax;
        ax.data_size = 16;
        ax.type = OPR_REG;
        ax.addr = REG_AX;
        ax.val = res;
        operand_write(&ax);
        if (((res >> 8) & 0xff) == 0)
        {
                cpu.eflags.CF = cpu.eflags.OF = 0;
        }
        else
        {
                cpu.eflags.CF = cpu.eflags.OF = 1;
        }
        print_asm_2("mul", "b", len, &rm, &al);
        return len;
}

make_instr_func(mul_rm2a_v)
{
        int len = 1; // in group
        OPERAND a, rm;
        a.data_size = data_size;
        a.type = OPR_REG;
        a.addr = REG_EAX;
        rm.data_size = data_size;
        len += modrm_rm(eip + 1, &rm);
        operand_read(&a);
        operand_read(&rm);
        OPERAND ax, dx;
        dx.type = ax.type = OPR_REG;
        ax.addr = REG_AX;
        dx.addr = REG_DX;
        if (data_size == 16)
        {
                uint32_t res = alu_mul(rm.val, a.val, 16);
                dx.data_size = ax.data_size = 16;
                ax.val = res & 0xffff;
                dx.val = (res >> 16) & 0xffff;
                print_asm_3("mul", "w", len, &rm, &dx, &ax);
        }
        else
        { // data_size == 32
                uint64_t res = alu_mul(rm.val, a.val, 32);
                dx.data_size = ax.data_size = 32;
                ax.val = res & 0xffffffff;
                dx.val = (res >> 32) & 0xffffffff;
                print_asm_3("mul", "l", len, &rm, &dx, &ax);
        }
        operand_write(&ax);
        operand_write(&dx);
        if (dx.val == 0)
        {
                cpu.eflags.CF = cpu.eflags.OF = 0;
        }
        else
        {
                cpu.eflags.CF = cpu.eflags.OF = 1;
        }
        return len;
}
/*
make_instr_func(imul_rm2r_v)
{
    int len = 1;
    OPERAND r, rm;
    r.data_size = data_size;
    r.type = OPR_REG;
    rm.data_size = data_size;
    len += modrm_r_rm(eip + 1, &r, &rm);
    
    operand_read(&r);
    operand_read(&rm);
    
    print_asm_2("imul", "v", len, &rm, &r);
    
    int64_t res = alu_imul(rm.val, r.val, data_size);
    r.val = res;
    operand_write(&r);
    
    return len;
}

make_instr_func(imul_rm2a_b)
{
    int len = 1; // in group
    OPERAND al, rm;
    al.data_size = 8;
    al.type = OPR_REG;
    al.addr = REG_AL;
    rm.data_size = 8;
    len += modrm_rm(eip + 1, &rm);
    operand_read(&al);
    operand_read(&rm);
    // zero extent
    uint32_t res = alu_imul(rm.val, al.val, 8);
    OPERAND ax;
    ax.data_size = 16;
    ax.type = OPR_REG;
    ax.addr = REG_AX;
    ax.val = res;
    operand_write(&ax);
    print_asm_2("imul", "b", len, &rm, &al);
    return len;
}

make_instr_func(imul_rm2a_v)
{
    int len = 1; // in group
    OPERAND a, rm;
    a.data_size = data_size;
    a.type = OPR_REG;
    a.addr = REG_EAX;
    rm.data_size = data_size;
    len += modrm_rm(eip + 1, &rm);
    operand_read(&a);
    operand_read(&rm);
    OPERAND ax, dx;
    dx.type = ax.type = OPR_REG;
    ax.addr = REG_AX;
    dx.addr = REG_DX;
    if (data_size == 16)
    {
        int32_t res = alu_imul(rm.val, a.val, 16);
        dx.data_size = ax.data_size = 16;
        ax.val = res & 0xffff;
        dx.val = (res >> 16) & 0xffff;
        print_asm_3("imul", "w", len, &rm, &dx, &ax);
    }
    else
    { // data_size == 32
            int64_t res = alu_imul(rm.val, a.val, 32);
            dx.data_size = ax.data_size = 32;
            ax.val = res & 0xffffffff;
            dx.val = (res >> 32) & 0xffffffff;
            print_asm_3("imul", "l", len, &rm, &dx, &ax);
    }
    operand_write(&ax);
    operand_write(&dx);
    
    return len;
}
make_instr_func(imul_irm2r_v)
{
    int len = 1; // in group
    OPERAND imm, rm, r;
    
    rm.data_size = data_size;
    
    r.data_size = data_size;
    r.type = OPR_REG;
    
    len += modrm_r_rm(eip + 1, &r, &rm);
    
    imm.data_size = data_size;
    imm.type = OPR_IMM;
    imm.addr = cpu.eip + len;
    
    operand_read(&imm);
    operand_read(&rm);
    
    r.val = alu_imul(imm.val, rm.val, data_size);
    operand_write(&r);
    
    return len + data_size / 8;
}

make_instr_func(imul_i8rm2r_v)
{
    int len = 1; // in group
    OPERAND imm, rm, r;
    
    rm.data_size = data_size;
    
    r.data_size = data_size;
    r.type = OPR_REG;
    
    len += modrm_r_rm(eip + 1, &r, &rm);
    
    imm.data_size = 8;
    imm.type = OPR_IMM;
    imm.addr = cpu.eip + len;
    
    operand_read(&imm);
    operand_read(&rm);
    
    r.val = alu_imul(sign_ext(imm.val, imm.data_size), sign_ext(rm.val, rm.data_size), data_size);
    operand_write(&r);
    
    return len + 1;
}
*/
/*
int64_t alu_imul(int32_t src, int32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_imul(src, dest, data_size);
#else
	uint64_t res = 0;
	res = (uint64_t)src * dest;
	uint32_t Lbits = 0;uint32_t Hbits = 0;
    switch (data_size)
    {
        case 8:
            Lbits = (res & 0xff);
            Hbits = ((res >> 8) & 0xff);
            cpu.eflags.CF = cpu.eflags.OF = 1;
            if(sign(sign_ext(Lbits, data_size)) == 0)
            {
                if(Hbits == 0)
                {
                    cpu.eflags.CF = cpu.eflags.OF = 0;
                }
            }
            if(sign(sign_ext(Lbits, data_size)) == 1)
            {
                if(Hbits == 0xff)
                {
                    cpu.eflags.CF = cpu.eflags.OF = 0;
                }
            }
            break;
        case 16:
            Lbits = (res & 0xffff);
            Hbits = ((res >> 16) & 0xffff);
            cpu.eflags.CF = cpu.eflags.OF = 1;
            if(sign(sign_ext(Lbits, data_size)) == 0)
            {
                if(Hbits == 0)
                {
                    cpu.eflags.CF = cpu.eflags.OF = 0;
                }
            }
            if(sign(sign_ext(Lbits, data_size)) == 1)
            {
                if(Hbits == 0xffff)
                {
                    cpu.eflags.CF = cpu.eflags.OF = 0;
                }
            }
            break;
        default:
            Lbits = (res & 0xffffffff);
            Hbits = ((res >> 32) & 0xffffffff);
            cpu.eflags.CF = cpu.eflags.OF = 1;
            if(sign(sign_ext(Lbits, data_size)) == 0)
            {
                if(Hbits == 0)
                {
                    cpu.eflags.CF = cpu.eflags.OF = 0;
                }
            }
            if(sign(sign_ext(Lbits, data_size)) == 1)
            {
                if(Hbits == 0xffffffff)
                {
                    cpu.eflags.CF = cpu.eflags.OF = 0;
                }
            }
            break;
    }
    set_ZF_mul(res, data_size);
    set_SF_mul(res, data_size);
	uint32_t EAX = (uint32_t)res;
	set_PF(EAX);
    //set_AF();
    return res; 
#endif
}
*/