#include "cpu/cpu.h"
#include "memory/memory.h"

// return the linear address from the virtual address and segment selector
uint32_t segment_translate(uint32_t offset, uint8_t sreg)
{
	/* TODO: perform segment translation from virtual address to linear address
	 * by reading the invisible part of the segment register 'sreg'
	 */
	
	assert(0 <= sreg && sreg < 6);
	/*
    if(cpu.segReg[sreg].granularity == 0)
    {
        assert(offset <= cpu.segReg[sreg].limit);
    }
	else
	{
	    assert(offset <= cpu.segReg[sreg].limit << 12);
	}
    */
	assert(offset <= 0xFFFFFFFF);
	return cpu.segReg[sreg].base + offset;
}

// load the invisible part of a segment register
void load_sreg(uint8_t sreg)
{
	/* TODO: load the invisibile part of the segment register 'sreg' by reading the GDT.
	 * The visible part of 'sreg' should be assigned by mov or ljmp already.
	 */
	assert(0 <= sreg && sreg < 6);

	SegReg selector = cpu.segReg[sreg];
	uint32_t index = selector.index;
	uint32_t rpl = selector.rpl;
	uint32_t ti = selector.ti;
	assert(ti == 0);
	uint32_t base = cpu.gdtr.base;
	uint32_t limit = cpu.gdtr.limit;
	/* if(...) */
	//assert(index <= limit);
	SegDesc segdesc;

	segdesc.val[0] = laddr_read( base + 8 * index, 4);
	segdesc.val[1] = laddr_read( base + 8 * index + 4, 4);
	base = (segdesc.base_31_24 << 24) + (segdesc.base_23_16 << 16) + (segdesc.base_15_0);
	limit = (segdesc.limit_19_16 << 16) + (segdesc.limit_15_0);

	assert(base == 0);
	assert(limit == 0xFFFFF );
	assert(segdesc.granularity == 1);
	assert(rpl <= segdesc.privilege_level);
	
	cpu.segReg[sreg].base = base;
	cpu.segReg[sreg].limit = limit;
	return ;
	
	
}
