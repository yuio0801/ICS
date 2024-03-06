#include "common.h"
#include "memory.h"
#include <string.h>

#define VMEM_ADDR 0xa0000
#define SCR_SIZE (320 * 200)
#define NR_PT ((SCR_SIZE + PT_SIZE - 1) / PT_SIZE) // number of page tables to cover the vmem

PDE *get_updir();
//extern PDE kpdir[NR_PDE] align_to_page;
//extern PTE kptable[PHY_MEM / PAGE_SIZE] align_to_page;

void create_video_mapping()
{

	/* TODO: create an identical mapping from virtual memory area
	 * [0xa0000, 0xa0000 + SCR_SIZE) to physical memeory area
	 * [0xa0000, 0xa0000 + SCR_SIZE) for user program. You may define
	 * some page tables to create this mapping.
	 */

	//panic("please implement me");
	PDE *pdir = get_updir();
	PTE *ptable;
    for(uint32_t addr = VMEM_ADDR; addr < VMEM_ADDR + SCR_SIZE; addr += (1<<22))
    {
        ptable = (PTE*)(pdir[addr >> 22].page_frame << 12);
        for (; addr < VMEM_ADDR + SCR_SIZE && ((addr >> 12) & 0x3ff) <= 0x3ff; addr += PAGE_SIZE) 
        {
		    ptable[(addr >> 12) & 0x3ff].val = make_pte(addr);
		}
    }
    return ;
}

void video_mapping_write_test()
{
	int i;
	uint32_t *buf = (void *)VMEM_ADDR;
	for (i = 0; i < SCR_SIZE / 4; i++)
	{
		buf[i] = i;
	}
}

void video_mapping_read_test()
{
	int i;
	uint32_t *buf = (void *)VMEM_ADDR;
	for (i = 0; i < SCR_SIZE / 4; i++)
	{
		assert(buf[i] == i);
	}
}

void video_mapping_clear()
{
	memset((void *)VMEM_ADDR, 0, SCR_SIZE);
}
