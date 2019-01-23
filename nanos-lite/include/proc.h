#ifndef __PROC_H__
#define __PROC_H__

#include "common.h"
#include "memory.h"

int fs_open(const char *, int, size_t);
size_t get_ramdisk_size(void);
size_t fs_filesz(int);
size_t fs_read(int, void*, size_t);
#define STACK_SIZE (8 * PGSIZE)

typedef union {
  uint8_t stack[STACK_SIZE] PG_ALIGN;
  struct {
    _Context *cp;
    _Protect as;
    uintptr_t cur_brk;
    // we do not free memory, so use `max_brk' to determine when to call _map()
    uintptr_t max_brk;
  };
} PCB;

extern PCB *current;

void context_kload(PCB *, void *);
void context_uload(PCB *, const char *);
#endif
