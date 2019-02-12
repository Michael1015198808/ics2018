#include "proc.h"

void naive_uload(PCB *, const char *);
#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used));
static PCB pcb_boot;
PCB *current;
static int fg_pcb=1;//Front ground PCB
//switch between pcb[0] and pcb[fg_pcb]

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
  context_uload(&pcb[1], "/bin/pal");
  context_uload(&pcb[2], "/bin/dummy");
  context_uload(&pcb[3], "/bin/init");
  switch_boot_pcb();
}

_Context* schedule(_Context *prev) {
    current->cp=prev;
    static uint8_t cnt=0;
    current = (++cnt==0 ? &pcb[0] : &pcb[fg_pcb]);
    static int fd=-1;
    if(fd==-1){
        fd=fs_open("/dev/events",0,0);
    }
    char info[10];
    fs_read(fd,info,10);
    return current->cp;
}
