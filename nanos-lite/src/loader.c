#include "proc.h"

#include <stdlib.h>
#define DEFAULT_ENTRY 0x4000000

static uintptr_t loader(PCB *pcb, const char *filename) {
  size_t get_ramdisk_size();
  size_t size=get_ramdisk_size();
  int _open(const char *, int, mode_t);
  int file_no=_open(filename,0,0);
  Log("File no.%d\n",file_no);
  size_t ramdisk_read(void*, size_t, size_t);
  ramdisk_read((void*)DEFAULT_ENTRY,0,size);
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

  pcb->tf = _kcontext(stack, entry, NULL);
}

void context_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);

  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->tf = _ucontext(&pcb->as, stack, stack, (void *)entry, NULL);
}
