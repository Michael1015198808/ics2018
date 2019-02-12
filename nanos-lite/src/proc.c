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
char *program[]={
  "/bin/hello",
  "/bin/pal",
  "/bin/dummy",
  "/bin/init"
};
void init_proc() {
  //context_kload(&pcb[1],(void*)hello_fun);
  int i;
  for(i=0;i<4;++i){
    context_uload(&pcb[i],program[i]);
  }
  switch_boot_pcb();
}

_Context* schedule(_Context *prev) {
    current->cp=prev;
    static uint8_t cnt=0;
    current = (((++cnt)&15)==0 ? &pcb[0] : &pcb[fg_pcb]);
    static int fd=-1;
    if(fd==-1){
        fd=fs_open("/dev/events",0,0);
    }
    char info[25],con[10]="kd F\0\0";
    fs_read(fd,info,25);
    const char code[]={0xf1,0xc3};//For output
    int i;
    for(i=1;i<5;++i){
        con[4]='0'+i;
        if(!strncmp(info,con,5)){break;}
    }
    if(i<5)
      switch(i){
        case 1:
        case 2:
        case 3:
            ((void(*)(char*))code)("switch between hello and\n");
            ((void(*)(char*))code)(program[i]);
            fg_pcb=i;
            break;
        case 4:
            ((void(*)(char*))code)("reload hello\n");
            context_uload(&pcb[0], "/bin/hello");
      }
    return current->cp;
}
