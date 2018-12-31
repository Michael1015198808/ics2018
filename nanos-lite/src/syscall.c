#include "common.h"
#include "syscall.h"

_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;
  //Log("_call%p,%d,%d,%x,%d\n",c,c->GPR1,c->GPR2,c->GPR3,c->GPR4);

  switch (a[0]) {
    case SYS_exit:_halt(c->GPRx);printf("Should have exited\n");break;
    case SYS_yield:_yield();break;
    case SYS_open:c->GPRx=fs_open((char*)c->GPR2,c->GPR3,c->GPR4);break;
    case SYS_read:c->GPRx=fs_read(c->GPR2,(void*)c->GPR3,c->GPR4);break;
    case SYS_write:
      if(a[1]==1||a[1]==2){
        int i;
        for(i=0;i<a[3];++i){
          _putc(*((char*)a[2]+i));
        }
        c->GPRx=i;
      }
      break;
    case SYS_lseek:c->GPRx=fs_lseek(c->GPR2,c->GPR3,c->GPR4);break;
    case SYS_brk:
      c->GPRx=0;//Success
      break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
