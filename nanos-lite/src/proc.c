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
  //context_kload(&pcb[1],(void*)hello_fun);
  context_uload(&pcb[0], "/bin/hello");
  switch_boot_pcb();
}

_Context* schedule(_Context *prev) {
    Log("schedule");
    Log("0x%08x->0x%08x",prev,pcb[0].cp);
    current->cp=prev;
    current=&pcb[0];
    return pcb[0].cp;
    static uint8_t cnt=0;
    current = (++cnt==0 ? &pcb[1] : &pcb[0]);
    return current->cp;
}
void test(void){
    static void* test_ptr=NULL;
    static int cnt=0;
    ++cnt;
    if(test_ptr==NULL)test_ptr=pcb[0].cp->prot->ptr;
    if(test_ptr!=pcb->cp->prot->ptr){
        Log("%d",cnt);
        assert(0);
    }
}
