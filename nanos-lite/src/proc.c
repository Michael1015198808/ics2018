#include "proc.h"

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used));
static PCB pcb_boot;
PCB *current;

void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    Log("Hello World from Nanos-lite for the %dth time!", j);
    j ++;
    _yield();
  }
}

void init_proc() {
 void naive_uload(PCB *, const char *);
 context_kload(pcb,(void*)hello_fun);
 switch_boot_pcb();
  naive_uload(NULL, "/bin/init");
}

_Context* schedule(_Context *prev) {
    current->cp=prev;
    current=pcb;
    return current->cp;
}
