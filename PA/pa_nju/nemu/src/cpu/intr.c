#include "cpu/intr.h"
#include "cpu/instr.h"
#include "memory/memory.h"

typedef union InterruptDescriptor {
	struct
	{
		uint32_t offset_15_0 : 16;
		uint32_t selector : 16;
		uint32_t notused : 5;
		uint32_t type : 3;
		uint32_t interrupt_type : 5;
		uint32_t dpl : 2;
		uint32_t present : 1;
		uint32_t offset_31_16 : 16;
	};
	uint32_t val[2];
} ItrDesc;

static void push_(uint32_t x)
{
    opr_dest.type = OPR_MEM;
    opr_dest.sreg = SREG_SS;
    
    opr_dest.data_size = 32;
    opr_dest.addr = cpu.esp - 4; 
    opr_dest.mem_addr.disp = opr_dest.addr; 
    
    opr_dest.val = x;
    operand_write(&opr_dest);
    
    cpu.esp -= 4;
}

void raise_intr(uint8_t intr_no)
{
#ifdef IA32_INTR
//  	printf("Please implement raise_intr()");
//  	fflush(stdout);
  	//assert(0);
 	
  	uint32_t base = cpu.idtr.base;
	uint32_t offset, selector;
	
	
	ItrDesc itrdesc;

	itrdesc.val[0] = laddr_read( base + 8 * intr_no, 4);
	itrdesc.val[1] = laddr_read( base + 8 * intr_no + 4, 4);
	offset = (itrdesc.offset_31_16 << 16) + (itrdesc.offset_15_0);
	selector = itrdesc.selector;
	
	push_(cpu.eflags.val);
	push_(cpu.segReg[SREG_CS].val);
	push_(cpu.eip);
	if(itrdesc.interrupt_type == 0xe)
	{
	    cpu.eflags.IF = 0;
	}
	cpu.segReg[SREG_CS].val = selector;
	cpu.eip = offset;
	load_sreg(SREG_CS);
	return ;
#endif
}

void raise_sw_intr(uint8_t intr_no)
{
	// return address is the next instruction
	cpu.eip += 2;
	raise_intr(intr_no);
}
