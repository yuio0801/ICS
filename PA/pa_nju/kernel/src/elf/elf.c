#include "common.h"
#include "memory.h"
#include "string.h"

#include <elf.h>

#ifdef HAS_DEVICE_IDE
#define ELF_OFFSET_IN_DISK 0
#endif

#define STACK_SIZE (1 << 20)

void ide_read(uint8_t *, uint32_t, uint32_t);
void create_video_mapping();
uint32_t get_ucr3();

uint32_t loader()
{
	Elf32_Ehdr *elf;
	Elf32_Phdr *ph, *eph;

#ifdef HAS_DEVICE_IDE
    uint8_t *buf;
    buf = (void *) 0x0;
	ide_read(buf, ELF_OFFSET_IN_DISK, 52);
	
	elf = (void *)0x0;
	Log("ELF loading from hard disk.");
#else
	elf = (void *)0x0;
	Log("ELF loading from ram disk.");
#endif
    

	ph = (void *)elf + elf->e_phoff;
	eph = ph + elf->e_phnum;
	volatile uint32_t entry = elf->e_entry;
	for (; ph < eph; ph++)
	{
	    
	    ide_read(buf, (uint32_t)ph, 64);
	    Elf32_Phdr *p = (void *)buf;
		if (p->p_type == PT_LOAD)
		{
			// remove this panic!!!
			//panic("Please implement the loader");
			uint32_t paddr = p->p_vaddr;


            #ifdef IA32_PAGE

            			//panic("Please implement the loader");
            			
            			extern uint32_t brk;
            			uint32_t new_brk = p->p_vaddr + p->p_memsz - 1;
            			if (brk < new_brk)
            			{
;            				brk = new_brk;
            			}

            			paddr = mm_malloc(paddr, p->p_memsz);
            			//paddr = (uint32_t)pa_to_va(mm_malloc(paddr, ph->p_memsz));
            			//paddr = (((uint32_t)pa_to_va(mm_malloc(paddr, ph->p_memsz)))&0xfffff000) | (paddr & 0xfff) ;
            #endif
            
                uint32_t addr = ((uint32_t)elf + p->p_offset);
                uint32_t p_memsz = p->p_memsz;
                uint32_t p_filesz = p->p_filesz;
                
                ide_read(buf, addr, p_filesz);
                
//              memcpy((void*)paddr, (void*)((uint32_t)elf + p->p_offset), p->p_filesz);
            
// 			    memset((void*)(paddr + p->p_filesz), 0, p->p_memsz - p->p_filesz);

                memcpy((void*)paddr, (void*)buf, p_filesz);
            
			    memset((void*)(paddr + p_filesz), 0, p_memsz - p_filesz);
			
			    //Log("elf: 0x%x 0x%x",paddr, p_filesz);
		}
	}

#ifdef IA32_PAGE
	mm_malloc(KOFFSET - STACK_SIZE, STACK_SIZE);
#ifdef HAS_DEVICE_VGA
	create_video_mapping();
#endif
	write_cr3(get_ucr3());
#endif
	return entry;
}


/*
#include "common.h"
#include "memory.h"
#include "string.h"

#include <elf.h>

#ifdef HAS_DEVICE_IDE
#define ELF_OFFSET_IN_DISK 0
#endif

#define STACK_SIZE (1 << 20)

void ide_read(uint8_t *, uint32_t, uint32_t);
void create_video_mapping();
uint32_t get_ucr3();

uint32_t loader()
{
	Elf32_Ehdr *elf;
	Elf32_Phdr *ph, *eph;

#ifdef HAS_DEVICE_IDE
    uint8_t *buf;
    buf = (void *) 0x0;
	ide_read(buf, ELF_OFFSET_IN_DISK, 40960);
	
	elf = (void *)0x0;
	Log("ELF loading from hard disk.");
#else
	elf = (void *)0x0;
	Log("ELF loading from ram disk.");
#endif
    

	ph = (void *)elf + elf->e_phoff;
	eph = ph + elf->e_phnum;
	volatile uint32_t entry = elf->e_entry;
	for (; ph < eph; ph++)
	{
	    Elf32_Phdr *p = (void *)ph;
		if (p->p_type == PT_LOAD)
		{
			// remove this panic!!!
			//panic("Please implement the loader");
			uint32_t paddr = p->p_vaddr;


            #ifdef IA32_PAGE

            			
            			extern uint32_t brk;
            			uint32_t new_brk = p->p_vaddr + p->p_memsz - 1;
            			if (brk < new_brk)
            			{
;            				brk = new_brk;
            			}

            			paddr = mm_malloc(paddr, p->p_memsz);
            			//paddr = (uint32_t)pa_to_va(mm_malloc(paddr, ph->p_memsz));
            			//paddr = (((uint32_t)pa_to_va(mm_malloc(paddr, ph->p_memsz)))&0xfffff000) | (paddr & 0xfff) ;
            #endif
            
                uint32_t addr = ((uint32_t)elf + p->p_offset);
                uint32_t p_memsz = p->p_memsz;
                uint32_t p_filesz = p->p_filesz;
                
                
//              memcpy((void*)paddr, (void*)((uint32_t)elf + p->p_offset), p->p_filesz);
            
// 			    memset((void*)(paddr + p->p_filesz), 0, p->p_memsz - p->p_filesz);

                memcpy((void*)paddr, (void*)addr, p_filesz);
            
			    memset((void*)(paddr + p_filesz), 0, p_memsz - p_filesz);
			
		}
	}

#ifdef IA32_PAGE
	mm_malloc(KOFFSET - STACK_SIZE, STACK_SIZE);
#ifdef HAS_DEVICE_VGA
	create_video_mapping();
#endif
	write_cr3(get_ucr3());
#endif
	return entry;
}

*/