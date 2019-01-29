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
#define addr_join(_A,_B)\
    ((\
      ((uint32_t)_A)&\
      (~0xfff))\
      +(_B<<2))
#define pde (((uintptr_t)cpu.CR3&-pow2(12)))
#define pte (paddr_read(addr_join(cpu.CR3,va.dir),4))
#define pa (paddr_read((pte&(~0xfff))+va.offset,4))
static inline paddr_t page_translate(vaddr_t addr){
  union{
      struct{
          uint32_t offset:12;
          uint32_t page:10;
          uint32_t dir:10;
      };
      uint32_t val;
  } va;
  va.val=addr;
  Log("%8x",addr);
  Log("%8lx",pde);
  Log("%8x",pte);
  Log("%8x",pa);
  *(int*)0=0;
  return pa;
}
#define GP CR0&0x80000000
#define CROSS_PAGE (((addr+len-1)&pow2(12))<len-1)
uint32_t vaddr_read(vaddr_t addr, int len) {
  if(cpu.GP){
    if(CROSS_PAGE){
      printf("Cross page!\n");
      assert(0);
    }else{
      return paddr_read(page_translate(addr), len);
    }
  }else{
      return paddr_read(addr, len);
  }
}

void vaddr_write(vaddr_t addr, uint32_t data, int len) {
  if(cpu.GP){
    if(CROSS_PAGE){
      printf("Cross page!\n");
      assert(0);
    }else{
      paddr_write(page_translate(addr), data, len);
    }
  }else{
    paddr_write(addr, data, len);
  }
}

