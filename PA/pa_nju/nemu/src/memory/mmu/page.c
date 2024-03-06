#include "cpu/cpu.h"
#include "memory/memory.h"

// translate from linear address to physical address
// typedef union{
//     struct{
//         uint32_t offset:12;
//         uint32_t page:10;
//         uint32_t dir:10;
//     };
//     uint32_t val;
// }laddr_u;
paddr_t page_translate(laddr_t laddr)
{
#ifndef TLB_ENABLED 
// 	printf("\nPlease implement page_translate()\n");
// 	fflush(stdout);
// 	assert(0);
    //  if(0xa0000<=laddr&&laddr<0xa0000 + 320*200)
    //  {
    //      return laddr;
    //  }
    PDE pde;
    pde.val = paddr_read((cpu.cr3.pdbr << 12) + 4 * (laddr >> 22), 4);
    assert(pde.present == 1);
   
    PTE pte;
    pte.val = paddr_read((pde.page_frame << 12) + 4 * ((laddr >> 12) & 0x3ff), 4);
    assert(pte.present == 1);
    
    if(pde.present == 0 && pte.present == 0)
    {   
        printf("nemu pde:0x%x\n",pde.val);
        printf("nemu pte:0x%x\n",pte.val);
        printf("error:0x%x\n",laddr);
        printf("pbdr:0x%x\n",cpu.cr3.pdbr << 12);
    }
    return ((pte.page_frame << 12) + (laddr & 0xfff));
    
    // laddr_u addr ;
    // addr.val = (uint32_t)laddr;
    // uint32_t pgdir = cpu.cr3.pdbr << 12;
    // uint32_t pgtbl = paddr_read(pgdir + addr.dir * 4, 4);
    // assert(pgtbl & 1);
    // pgtbl &= 0xfffff000;
    // uint32_t paddr = paddr_read(pgtbl + addr.page * 4, 4);
    // assert(paddr & 1);
    // return (paddr & 0xfffff000)|addr.offset;
#else
	return tlb_read(laddr) | (laddr & PAGE_MASK);
#endif
}
