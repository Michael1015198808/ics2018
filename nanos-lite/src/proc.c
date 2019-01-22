#include "proc.h"

void naive_uload(PCB *, const char *);
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
 Log("init_proc");
 context_kload(pcb,(void*)hello_fun);
 Log("kload finished");
 switch_boot_pcb();
 Log("Switch boot pcb finished");
  //naive_uload(NULL, "/bin/init");
}

_Context* schedule(_Context *prev) {
    Log("schedule start");
    current->cp=prev;
    current=pcb;
    Log("schedule return");
    return current->cp;
}
