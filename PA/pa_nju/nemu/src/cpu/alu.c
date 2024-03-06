#include "cpu/cpu.h"
void set_CF_add(uint32_t result, uint32_t src, size_t data_size)
{
    result = sign_ext(result & (0xffffffff >> (32 - data_size)), data_size);
    src = sign_ext(src & (0xffffffff >> (32 - data_size)), data_size);
    cpu.eflags.CF = (result < src);
}
void set_CF_adc(uint32_t result, uint32_t src, size_t data_size)
{
    result = sign_ext(result & (0xffffffff >> (32 - data_size)), data_size);
    src = sign_ext(src & (0xffffffff >> (32 - data_size)), data_size);
    if(cpu.eflags.CF == 1)
    {
        cpu.eflags.CF = (result <= src);
    }
    else
    {
        cpu.eflags.CF = (result < src);
    }
}
void set_CF_sub(uint32_t src, uint32_t dest, size_t data_size)
{
    dest = sign_ext(dest & (0xffffffff >> (32 - data_size)), data_size);
    src = sign_ext(src & (0xffffffff >> (32 - data_size)), data_size);
    cpu.eflags.CF = ((uint32_t)dest < (uint32_t)src);
}
void set_CF_sbb(uint32_t src, uint32_t dest, size_t data_size)
{
    dest = sign_ext(dest & (0xffffffff >> (32 - data_size)), data_size);
    src = sign_ext(src & (0xffffffff >> (32 - data_size)), data_size);
    uint32_t CF = cpu.eflags.CF;
    if(CF == 1)
    {
        cpu.eflags.CF = ((uint32_t)dest <= (uint32_t)src);
    }
    else
    {
        cpu.eflags.CF = ((uint32_t)dest < (uint32_t)src);
    }
}
void set_ZF(uint32_t result, size_t data_size)
{
    result = result & (0xffffffff >> (32 - data_size));
    cpu.eflags.ZF = (result == 0);
}
void set_SF(uint32_t result, size_t data_size)
{
    result = sign_ext(result & (0xffffffff >> (32 - data_size)), data_size);
    cpu.eflags.SF = sign(result);
}

void set_ZF_mul(uint64_t result, size_t data_size)
{
    result = result & (0xffffffffffffffff >> (64 - data_size * 2));
    cpu.eflags.ZF = (result == 0);
}
void set_SF_mul(uint64_t result, size_t data_size)
{
    cpu.eflags.SF = ((result >> (data_size * 2 - 1)) & 1);
}

void set_PF(uint32_t result)
{
    int32_t sum = 0;
    size_t data_size = 8;
    while(data_size)
    {
        int32_t d = (result & 1);
        sum += d;
        result >>= 1;
        data_size --;
    }
    cpu.eflags.PF = (sum % 2 == 0);
}
void set_OF_add(uint32_t result, uint32_t src, uint32_t dest, size_t data_size)
{
    switch(data_size)
    {
        case 8:
            result = sign_ext(result & 0xff, 8);
            src = sign_ext(src & 0xff, 8);
            dest = sign_ext(dest & 0xff, 8);
            break;
        case 16:
            result = sign_ext(result & 0xffff, 16);
            src = sign_ext(src & 0xffff, 16);
            dest = sign_ext(dest & 0xffff, 16);
            break;
        default: break;
    }
    if(sign(src) == sign(dest))
    {
        if(sign(src) != sign(result))
        {
            cpu.eflags.OF = 1;
        }
        else
        {
            cpu.eflags.OF = 0;
        }
    }
    else
    {
        cpu.eflags.OF = 0;
    }
}
void set_OF_sub(uint32_t result, uint32_t src, uint32_t dest, size_t data_size)
{
    switch(data_size)
    {
        case 8:
            result = sign_ext(result & 0xff, 8);
            src = sign_ext(src & 0xff, 8);
            dest = sign_ext(dest & 0xff, 8);
            break;
        case 16:
            result = sign_ext(result & 0xffff, 16);
            src = sign_ext(src & 0xffff, 16);
            dest = sign_ext(dest & 0xffff, 16);
            break;
        default: break;
    }
    if(sign(src) != sign(dest))
    {
        if(sign(dest) != sign(result))
        {
            cpu.eflags.OF = 1;
        }
        else
        {
            cpu.eflags.OF = 0;
        }
    }
    else
    {
        cpu.eflags.OF = 0;
    }
}
uint32_t alu_add(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_add(src, dest, data_size);
#else
	uint32_t res = 0;
	res = dest + src;
	
	set_CF_add(res, src, data_size);
	set_PF(res);
	//set_AF();
	set_ZF(res, data_size);
	set_SF(res, data_size);
	set_OF_add(res, src, dest, data_size);
	
	return res & (0xffffffff >> (32 - data_size));
#endif
}

uint32_t alu_adc(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_adc(src, dest, data_size);
#else
	uint32_t res = 0;
	res = dest + src + cpu.eflags.CF;
	
	set_CF_adc(res, src, data_size);
	set_PF(res);
	//set_AF();
	set_ZF(res, data_size);
	set_SF(res, data_size);
	set_OF_add(res, src, dest, data_size);
	
	return res & (0xffffffff >> (32 - data_size));
#endif
}

uint32_t alu_sub(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_sub(src, dest, data_size);
#else
	uint32_t res = 0;
	res = dest + ~src + 1;
	
	set_CF_sub(src, dest, data_size);
	set_PF(res);
	//set_AF();
	set_ZF(res, data_size);
	set_SF(res, data_size);
	set_OF_sub(res, src, dest, data_size);
	
	return res & (0xffffffff >> (32 - data_size));
#endif
}

uint32_t alu_sbb(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_sbb(src, dest, data_size);
#else
    uint32_t res = dest + ~(src + cpu.eflags.CF) + 1;
    
    set_CF_sbb(src, dest, data_size);
    set_PF(res);
    //set_AF();
	set_ZF(res, data_size);
	set_SF(res, data_size);
	set_OF_sub(res, src, dest, data_size);
    
    return res & (0xffffffff >> (32 - data_size));
#endif
}

uint64_t alu_mul(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_mul(src, dest, data_size);
#else
    uint64_t res = 0;
	res = (uint64_t)src * dest;
    switch (data_size)
    {
        case 8:
            cpu.eflags.CF = cpu.eflags.OF = (((res >> 8) & (0xff)) != 0);
            break;
        case 16:
            cpu.eflags.CF = cpu.eflags.OF = (((res >> 16) & (0xffff)) != 0);
            break;
        default:
            cpu.eflags.CF = cpu.eflags.OF = (((res >> 32) & (0xffffffff)) != 0);
            break;
    }
    set_ZF_mul(res, data_size);
        set_SF_mul(res, data_size);
	uint32_t EAX = (uint32_t)res;
	set_PF(EAX);
    //set_AF();
    return res; //& (0xffffffffffffffff >> (64 - data_size));
#endif
}

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

// need to implement alu_mod before testing
uint32_t alu_div(uint64_t src, uint64_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_div(src, dest, data_size);
#else
	uint32_t res = 0;
	res = dest / src;
	return res;
#endif
}

// need to implement alu_imod before testing
int32_t alu_idiv(int64_t src, int64_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_idiv(src, dest, data_size);
#else
	int32_t res = 0;
	res = dest / src;
	return res;
#endif
}

uint32_t alu_mod(uint64_t src, uint64_t dest)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_mod(src, dest);
#else
	uint32_t res = 0;
	res = dest % src;
	return res;
#endif
}

int32_t alu_imod(int64_t src, int64_t dest)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_imod(src, dest);
#else
	int32_t res = 0;
	res = dest % src;
	return res;
#endif
}

uint32_t alu_and(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_and(src, dest, data_size);
#else
	uint32_t res = 0;
	res = src & dest;
	cpu.eflags.CF = 0;
	cpu.eflags.OF = 0;
	set_PF(res);
	//set_AF();
	set_ZF(res, data_size);
	set_SF(res, data_size);
	
	return res & (0xffffffff >> (32 - data_size));
#endif
}

uint32_t alu_xor(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_xor(src, dest, data_size);
#else
	uint32_t res = 0;
	res = src ^ dest;
	cpu.eflags.CF = 0;
	cpu.eflags.OF = 0;
	set_PF(res);
	//set_AF();
	set_ZF(res, data_size);
	set_SF(res, data_size);
	
	return res & (0xffffffff >> (32 - data_size));
#endif
}

uint32_t alu_or(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_or(src, dest, data_size);
#else
	uint32_t res = 0;
	res = src | dest;
	cpu.eflags.CF = 0;
	cpu.eflags.OF = 0;
	set_PF(res);
	//set_AF();
	set_ZF(res, data_size);
	set_SF(res, data_size);
	
	return res & (0xffffffff >> (32 - data_size));
#endif
}

uint32_t alu_shl(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_shl(src, dest, data_size);
#else
    src = src & (0xffffffff >> 27);
    uint32_t res = dest;
    while(src)
    {
        cpu.eflags.OF = (cpu.eflags.CF == (sign(sign_ext(res, data_size))));
        cpu.eflags.CF = sign(sign_ext(res, data_size));
        res <<= 1;
        src--;
    }
    set_ZF(res, data_size);
    set_SF(res, data_size);
    set_PF(res);
	return res & (0xffffffff >> (32 - data_size));
#endif
}

uint32_t alu_shr(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_shr(src, dest, data_size);
#else
	src = src & (0xffffffff >> 27);
    uint32_t res = dest & (0xffffffff >> (32 - data_size));
    while(src)
    {
        cpu.eflags.OF = (cpu.eflags.CF == (res & 1));
        cpu.eflags.CF = res & 1;
        res >>= 1;
        src--;
    }
    set_ZF(res, data_size);
    set_SF(res, data_size);
    set_PF(res);
	return res & (0xffffffff >> (32 - data_size));
#endif
}

uint32_t alu_sar(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_sar(src, dest, data_size);
#else
	src = src & (0xffffffff >> 27);
    int32_t res = sign_ext(dest & (0xffffffff >> (32 - data_size)), data_size);
    while(src)
    {
        cpu.eflags.OF = (cpu.eflags.CF == (res & 1));
        cpu.eflags.CF = res & 1;
        res >>= 1;
        src--;
    }
    set_ZF(res, data_size);
    set_SF(res, data_size);
    set_PF(res);
	return res & (0xffffffff >> (32 - data_size));
#endif
}

uint32_t alu_sal(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_sal(src, dest, data_size);
#else
    
    src = src & (0xffffffff >> 27);
    uint32_t res = dest;
    while(src)
    {
        cpu.eflags.OF = (cpu.eflags.CF == (sign(sign_ext(res, data_size))));
        cpu.eflags.CF = sign(sign_ext(res, data_size));
        res <<= 1;
        src--;
    }
    set_ZF(res, data_size);
    set_SF(res, data_size);
    set_PF(res);
	return res & (0xffffffff >> (32 - data_size));
#endif
}
