#include "cpu/instr.h"
#include "device/port_io.h"

make_instr_func(in_b)
{
    uint32_t res = pio_read(cpu.edx, 1);
    cpu.eax = res;
    return 1;
}

make_instr_func(in_v)
{
    uint32_t res = pio_read(cpu.edx, data_size / 8);
    cpu.eax = res;
    return 1;
}

/*
Put the implementations of `in' instructions here.
*/
