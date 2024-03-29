#include "nemu.h"

#define PMEM_SIZE (128 * 1024 * 1024)

#define pmem_rw(addr, type) *(type *)({\
    Assert(addr < PMEM_SIZE, "physical address(0x%08x) is out of bound", addr); \
    guest_to_host(addr); \
    })

uint8_t pmem[PMEM_SIZE];

uint32_t mmio_read(paddr_t,int,int);
void mmio_write(paddr_t,int,uint32_t,int);
int is_mmio(paddr_t);
/* Memory accessing interfaces */

uint32_t paddr_read(paddr_t addr, int len) {
	if(is_mmio(addr)==-1){
    //printf("%x\n",cpu.eip);
  	return pmem_rw(addr, uint32_t) & (~0u >> ((4 - len) << 3));
	}else{
		return mmio_read(addr,len,is_mmio(addr));
	}
}

void paddr_write(paddr_t addr, uint32_t data, int len) {
	if(is_mmio(addr)==-1){
  	memcpy(guest_to_host(addr), &data, len);
	}else{
		mmio_write(addr,len,data,is_mmio(addr));
	}
}

uint32_t vaddr_read(vaddr_t addr, int len) {
  return paddr_read(addr, len);
}

void vaddr_write(vaddr_t addr, uint32_t data, int len) {
  paddr_write(addr, data, len);
}

