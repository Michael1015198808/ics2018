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
#define pow2(_num) (1<<(_num))
#define CROSS_PAGE (((addr+len-1)&pow2(12))<len-1)
#define voffset (va-0)
#define pde ((uint32_t*)(uintptr_t)cpu.CR3)
#define pde_idx ((voffset>>22)&-(pow2(32-10)))
#define pte ((uint32_t*)(uintptr_t)(pde[pde_idx]&-(pow2(12))))
#define pte_idx ((voffset>>12)&(-pow2(32-10)))
static inline paddr_t page_translate(vaddr_t va){
  Log("translate");
  Log("%d",cpu.CR3);
  Log("%d",pde[pde_idx]);
  Log("%d",pte[pte_idx]);
  paddr_t pa=pte[pte_idx]+(va&(pow2(12)-1));
  printf("%d->%d\n",va,pa);
  return pa;
}
uint32_t vaddr_read(vaddr_t addr, int len) {
  if(CROSS_PAGE){
      printf("Cross page!\n");
      assert(0);
  }else{
    return paddr_read(page_translate(addr), len);
  }
}

void vaddr_write(vaddr_t addr, uint32_t data, int len) {
  if(CROSS_PAGE){
      printf("Cross page!\n");
      assert(0);
  }else{
    paddr_write(page_translate(addr), data, len);
  }
}

