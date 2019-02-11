#include <x86.h>

#define PG_ALIGN __attribute((aligned(PGSIZE)))

static PDE kpdirs[NR_PDE] PG_ALIGN;
static PTE kptabs[PMEM_SIZE / PGSIZE] PG_ALIGN;
static void* (*pgalloc_usr)(size_t);
static void (*pgfree_usr)(void*);

_Area segments[] = {      // Kernel memory mappings
  {.start = (void*)0,          .end = (void*)PMEM_SIZE}
};

#define NR_KSEG_MAP (sizeof(segments) / sizeof(segments[0]))

int _vme_init(void* (*pgalloc_f)(size_t), void (*pgfree_f)(void*)) {
  pgalloc_usr = pgalloc_f;
  pgfree_usr = pgfree_f;

  int i;

  // make all PDEs invalid
  for (i = 0; i < NR_PDE; i ++) {
    kpdirs[i] = 0;
  }

  PTE *ptab = kptabs;
  for (i = 0; i < NR_KSEG_MAP; i ++) {
    uint32_t pdir_idx = (uintptr_t)segments[i].start / (PGSIZE * NR_PTE);
    uint32_t pdir_idx_end = (uintptr_t)segments[i].end / (PGSIZE * NR_PTE);
    for (; pdir_idx < pdir_idx_end; pdir_idx ++) {
      // fill PDE
      kpdirs[pdir_idx] = (uintptr_t)ptab | PTE_P;

      // fill PTE
      PTE pte = PGADDR(pdir_idx, 0, 0) | PTE_P;
      PTE pte_end = PGADDR(pdir_idx + 1, 0, 0) | PTE_P;
      for (; pte < pte_end; pte += PGSIZE) {
        *ptab = pte;
        ptab ++;
      }
    }
  }

  set_cr3(kpdirs);
  set_cr0(get_cr0() | CR0_PG);

  return 0;
}

int _protect(_Protect *p) {
  PDE *updir = (PDE*)(pgalloc_usr(1));
  p->pgsize = 4096;
  p->ptr = updir;
  // map kernel space
  for (int i = 0; i < NR_PDE; i ++) {
    updir[i] = kpdirs[i];
  }

  p->area.start = (void*)0x8000000;
  p->area.end = (void*)0xc0000000;
  return 0;
}

void _unprotect(_Protect *p) {
}

_Protect *cur_as = NULL;
void get_cur_as(_Context *c) {
  c->prot = cur_as;
}

void _switch(_Context *c) {
  set_cr3(c->prot->ptr);
  cur_as = c->prot;
}

#define pow2(_num) (1<<(_num))
#define join_read(_A,_B) (*(uint32_t*)addr_join(_A,_B))
#define join_write(_A,_B,_C) (*(uint32_t*)(addr_join(_A,_B))=_C)
#define addr_join(_A,_B) \
    (((_A)<<(32-20)) \
      +(_B))
#define declare_Va \
union{ \
   struct{ \
     uint32_t offset:12; \
     uint32_t page:10; \
     uint32_t dir:10; \
   }; \
   uint32_t val; \
  } Va;
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
  } pde;
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
  } pte;

int _map(_Protect *p, void *va, void *pa, int mode) {
//#define prot ((PDE)(mode))
  const char code[]={0xf1,0xc3};
  ((void(*)(char*,...))code)("",va);
  if(mode==0)return 0;
  declare_Va;declare_pde;declare_pte;//Macros
  Va.val=(uintptr_t)va;
  pde.val=
    join_read(((uintptr_t)p->ptr)>>12,Va.dir<<2);
  if(!pde.present){
    join_write(((uintptr_t)p->ptr)>>12,Va.dir<<2,
            pde.val=((uintptr_t)pgalloc_usr(1))|PTE_P|mode);
  }
  pte.val=
    join_read(pde.page_frame,Va.page<<2);
  if(!pte.present){
    join_write(pde.page_frame,Va.page<<2,
            pte.val=(((uintptr_t)pa)&(-pow2(32-20)))|PTE_P|mode);
  }
  if((uintptr_t)pa!=addr_join(pte.page_frame,Va.offset)){
      const char code[]={0xf1,0xc3};
      ((void(*)(char*))code)("map fails");
  };
  return 0;
}

_Context *_ucontext(_Protect *p, _Area ustack, _Area kstack, void *entry, void *args) {

  void* new_end = ustack.end - 16 * sizeof(uintptr_t);//argc,
                                                     //argv,
                                                     //envp,
                                                     //ret_addr
                                                     //4 in total
  new_end =(void*)
          (((uintptr_t)new_end) & (-16));//栈帧对齐

  while(ustack.end!=new_end){
      ustack.end-=sizeof(uintptr_t);
      *(uintptr_t*)ustack.end=0;
  }
//_kcontext
  _Context *c=(_Context*)(ustack.end)-1;

  *(_Context**)(ustack.start)=c;
  c->eip=(uintptr_t)entry;
  c->prot=p;
//_kcontext

  c->edi=0;
  c->cs=8;//For diff-test

  return c;
}
