#include "nemu.h"
#include "cpu/cpu.h"
#include "memory/memory.h"
#include "device/mm_io.h"
#include <memory.h>
#include <stdio.h>

extern int is_mmio(uint32_t hwaddr);

extern uint32_t mmio_read(uint32_t hwaddr, size_t len, int map_NO);
extern void mmio_write(uint32_t hwaddr, size_t len, uint32_t data, int map_NO);

uint8_t hw_mem[MEM_SIZE_B];

uint32_t hw_mem_read(paddr_t paddr, size_t len)
{
	uint32_t ret = 0;
	memcpy(&ret, hw_mem + paddr, len);
	return ret;
}

void hw_mem_write(paddr_t paddr, size_t len, uint32_t data)
{
	memcpy(hw_mem + paddr, &data, len);
}

uint32_t paddr_read(paddr_t paddr, size_t len)
{
	uint32_t ret = 0;
	#ifdef CACHE_ENABLED
	    ret = cache_read(paddr, len);
	#else
	if(is_mmio(paddr) != -1)
	{
	    ret = mmio_read(paddr, len, is_mmio(paddr));
	}
	else
	{
	    ret = hw_mem_read(paddr, len);
	}
	#endif
	//ret = hw_mem_read(paddr, len);
	return ret;
}

void paddr_write(paddr_t paddr, size_t len, uint32_t data)
{
    #ifdef CACHE_ENABLED
        cache_write(paddr, len, data);
    #else
    if(is_mmio(paddr) != -1)
	{
	    mmio_write(paddr, len, data, is_mmio(paddr));
	}
	else
	{
	    hw_mem_write(paddr, len, data);
	}
    #endif
	//hw_mem_write(paddr, len, data);
}

uint32_t laddr_read(laddr_t laddr, size_t len)
{
    assert(len == 1 || len == 2 || len == 4);
    if(cpu.cr0.pg == 1)
    {
         if((laddr >> 12) != ((laddr + len - 1) >> 12))
         {
            uint8_t data[4] = {0};
            for(int i=0;i<len;++i)
            {
                data[i] = (uint8_t)laddr_read(laddr + i, 1);
            }
            return *(uint32_t*)data;
         }
         else
         {
            paddr_t paddr = page_translate(laddr);
            return paddr_read(paddr, len);
         }
    }
    else
    {
        return paddr_read(laddr, len);
    }
	
}

void laddr_write(laddr_t laddr, size_t len, uint32_t data)
{
    assert(len == 1 || len == 2 || len == 4);
    if(cpu.cr0.pg == 1)
    {
         if((laddr >> 12) != ((laddr + len - 1) >> 12))
         {
            for(int i=0;i<len;++i)
            {
                laddr_write(laddr + i, 1, ((uint8_t*)(&data))[i]);
            }
         }
         else
         {
            paddr_t paddr = page_translate(laddr);
            return paddr_write(paddr, len, data);
        }
    }
    else
    {
        return paddr_write(laddr, len, data);
    }
	//paddr_write(laddr, len, data);
}
/*
uint32_t vaddr_read(vaddr_t vaddr, uint8_t sreg, size_t len)
{
	assert(len == 1 || len == 2 || len == 4);
	return laddr_read(vaddr, len);
}
*/
uint32_t vaddr_read(vaddr_t vaddr, uint8_t sreg, size_t len) {
	assert(len == 1 || len == 2 || len == 4);
#ifndef IA32_SEG
	return laddr_read(vaddr, len);
#else
	uint32_t laddr = vaddr;
	if( cpu.cr0.pe == 1 ) {
		laddr = segment_translate(vaddr, sreg);
	}
	return laddr_read(laddr, len);
#endif
}

void vaddr_write(vaddr_t vaddr, uint8_t sreg, size_t len, uint32_t data)
{
	assert(len == 1 || len == 2 || len == 4);
	laddr_write(vaddr, len, data);
}

void init_mem()
{
	// clear the memory on initiation
	memset(hw_mem, 0, MEM_SIZE_B);
	#ifdef CACHE_ENABLED
	    init_cache();
	#endif

#ifdef TLB_ENABLED
	make_all_tlb();
	init_all_tlb();
#endif
}

uint32_t instr_fetch(vaddr_t vaddr, size_t len)
{
	assert(len == 1 || len == 2 || len == 4);
	return vaddr_read(vaddr, SREG_CS, len);
}

uint8_t *get_mem_addr()
{
	return hw_mem;
}
