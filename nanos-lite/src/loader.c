#include "proc.h"

#include <stdlib.h>
#define DEFAULT_ENTRY 0x8048000

static uintptr_t loader(PCB *pcb, const char *filename) {
  int file_no=fs_open(filename,0,0);
  size_t size=fs_filesz(file_no);
  void* ppage=new_page(size>>12);
  fs_read(file_no,ppage,size);
  int i;
  for(i=0;i<size/PGSIZE;++i){
    _map(&(pcb->as),(void*)DEFAULT_ENTRY+PGSIZE,ppage+PGSIZE,1);
  }
  Log("Program start\n");
  return DEFAULT_ENTRY;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  ((void(*)())entry) ();
}

void context_kload(PCB *pcb, void *entry) {
  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _kcontext(stack, entry, NULL);
}

void context_uload(PCB *pcb, const char *filename) {
  _protect(&(pcb->as));
  uintptr_t entry = loader(pcb, filename);

  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _ucontext(&pcb->as, stack, stack, (void *)entry, NULL);
}
