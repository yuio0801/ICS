#include "memory/mmu/cache.h"
#include "memory/memory.h"
#include<stdlib.h>

#define BLOCK_SIZE 64
#define SET_NUM 128
#define WAY_OF_SET 8



typedef union{
    uint32_t val;
    struct{
        uint32_t index_in_block : 6;
        uint32_t index_of_set : 7;
        uint32_t index : 19;
    };
}Paddr;

typedef struct{
    uint32_t valid : 1;
    uint32_t index : 19;
    uint8_t data[BLOCK_SIZE];
}Cache_block;

typedef Cache_block Cache_set[WAY_OF_SET];
typedef Cache_set Cache[SET_NUM];
static Cache cache;

Cache_block* find_cache_block(Paddr paddr)
{
    uint32_t index_of_set = paddr.index_of_set;
    uint32_t index = paddr.index;
    Cache_block *set = cache[index_of_set];
    for(int i = 0; i < WAY_OF_SET; ++i)
    {
        if(set[i].valid && set[i].index == index)
        {
            return &set[i];
        }
    }
    return NULL;
}
Cache_block* find_available_cache_block(Paddr paddr)
{
    uint32_t index_of_set = paddr.index_of_set;
    Cache_block *set = cache[index_of_set];
    for(int i = 0; i < WAY_OF_SET; ++i)
    {
        if(!set[i].valid)
        {
            return &set[i];
        }
    }
    
    uint32_t index_in_set = rand() % WAY_OF_SET;
    return &set[index_in_set];
}
// init the cache
void init_cache()
{
	// implement me in PA 3-1
	//memset(cache, 0, sizeof(cache));
	for(int i = 0; i < SET_NUM; ++i)
	{
	    for(int j = 0; j < WAY_OF_SET; ++j)
	    {
	        cache[i][j].valid = 0;
	    }
	}
}

// write data to cache
void cache_write(paddr_t paddr, size_t len, uint32_t data)
{
	// implement me in PA 3-1
	Paddr addr;
	addr.val = paddr;
	if(addr.index_in_block + len > BLOCK_SIZE)
	{
	    for(int i = 0; i < len; ++i)
	    {
	        
	        cache_write(paddr + i, 1, (uint32_t)(((uint8_t*)&paddr)[i]));
	    }
	    return;
	}
	else
	{
	    Cache_block *block = find_cache_block(addr);
	    if(block)
	    {
	        memcpy(&(block->data[addr.index_in_block]), &data, len);
	    }
        hw_mem_write(paddr, len, data);
	    return ;
	}
}

// read data from cache
uint32_t cache_read(paddr_t paddr, size_t len)
{
	// implement me in PA 3-1
	uint32_t res = 0;
	Paddr addr;
	addr.val = paddr;
	if(addr.index_in_block + len > BLOCK_SIZE)
	{
	    for(int i = 0; i < len; ++i)
	    {
	        ((uint8_t*)&res)[i] = (uint8_t)cache_read(paddr + i, 1);
	    }
	    return res;
	}
	else
	{
	    
	    Cache_block *block = find_cache_block(addr);
	    if(block)
	    {
	        memcpy(&res, &(block->data[addr.index_in_block]), len);
	        //printf("case2:%x\n",res);
	        //return hw_mem_read(paddr, len);
	    }
	    else
	    {
	        block = find_available_cache_block(addr);
	        block->valid = 1;
	        block->index = addr.index;
	        uint32_t index_in_block = addr.index_in_block;
	        addr.index_in_block = 0;
	        memcpy(block->data, hwa_to_va(addr.val), BLOCK_SIZE);
	        memcpy(&res, &(block->data[index_in_block]), len);
	        //printf("case3:%x\n",res);
	        //return hw_mem_read(paddr, len);
	    }
	    return res;
	}
}

