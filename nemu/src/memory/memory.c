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
#define declare_va \
union{ \
   struct{ \
     uint32_t offset:12; \
     uint32_t page:10; \
     uint32_t dir:10; \
   }; \
   uint32_t val; \
  } va;
#define declare_pde \
  union{ \
    struct { \
      uint32_t present             : 1; \
      uint32_t read_write          : 1; \
      uint32_t user_supervisor     : 1; \
      uint32_t page_write_through  : 1; \
      uint32_t page_cache_disable  : 1; \
      uint32_t accessed            : 1; \
      uint32_t pad0                : 6; \
      uint32_t page_frame          : 20; \
    }; \
    uint32_t val; \
  } PDE;
#define declare_pte \
  union{ \
    struct { \
      uint32_t present             : 1; \
      uint32_t read_write          : 1; \
      uint32_t user_supervisor     : 1; \
      uint32_t page_write_through  : 1; \
      uint32_t page_cache_disable  : 1; \
      uint32_t accessed            : 1; \
      uint32_t dirty               : 1; \
      uint32_t pad0                : 1; \
      uint32_t global              : 1; \
      uint32_t pad1                : 3; \
      uint32_t page_frame          : 20; \
    }; \
    uint32_t val; \
  } PTE;
#define join_read(_A,_B) paddr_read(addr_join(_A,_B),4)
#define addr_join(_A,_B) \
    (((_A)<<(32-20)) \
      +(_B))
static inline paddr_t page_translate(vaddr_t addr){
  declare_va;declare_pde;declare_pte;

  va.val=addr;

  PDE.val=
      join_read(cpu.CR3>>12,va.dir);
  //assert(PDE.present);

  Log("%x",PDE.val);
  PTE.val=
      join_read(PDE.page_frame,va.page);
  //assert(PTE.present);
  Log("%x",PTE.val);

  paddr_t pa=join_read(PTE.page_frame,va.offset);
  assert(addr==pa);//temp test
 *(int*)0=0;
  return pa;
}
#define CROSS_PAGE (((addr+len-1)&pow2(12))<len-1)
uint32_t vaddr_read(vaddr_t addr, int len) {
  if((cpu.CR0&0x80000000)){
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
  if((cpu.CR0&0x80000000)){
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

