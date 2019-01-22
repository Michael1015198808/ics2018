#include "proc.h"

#include <stdlib.h>
#define DEFAULT_ENTRY 0x4000000

static uintptr_t loader(PCB *pcb, const char *filename) {
  int file_no=fs_open(filename,0,0);
  size_t size=fs_filesz(file_no);
  fs_read(file_no,(void*)DEFAULT_ENTRY,size);
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
  //Log("%x",pcb->cp->eip);
}

void context_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);

  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _ucontext(&pcb->as, stack, stack, (void *)entry, NULL);
}
