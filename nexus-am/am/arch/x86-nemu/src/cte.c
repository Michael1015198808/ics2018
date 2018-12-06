#include <am.h>
#include <x86.h>

static _Context* (*user_handler)(_Event, _Context*) = NULL;

void vectrap();
void vecnull();

_Context* irq_handle(_Context *tf) {
  _Context *next = tf;
	int printf(char*,...);
	printf("eax:%d\t",tf->eax);
	printf("ecx:%d\t",tf->ecx);
	printf("edx:%d\t",tf->edx);
	printf("ebx:%d\n",tf->ebx);
	printf("esp:%d\t",tf->esp);
	printf("ebp:%d\t",tf->ebp);
	printf("esi:%d\t",tf->esi);
	printf("edi:%d\n",tf->edi);
	printf("err:%d\n",tf->err);
  if (user_handler) {
    _Event ev;
    switch (tf->irq) {
      default: ev.event = _EVENT_ERROR; break;
    }

    next = user_handler(ev, tf);
    if (next == NULL) {
      next = tf;
    }
  }

  return next;
}

static GateDesc idt[NR_IRQ];

int _cte_init(_Context*(*handler)(_Event, _Context*)) {
  // initialize IDT
  for (unsigned int i = 0; i < NR_IRQ; i ++) {
    idt[i] = GATE(STS_TG32, KSEL(SEG_KCODE), vecnull, DPL_KERN);
  }

  // -------------------- system call --------------------------
  idt[0x81] = GATE(STS_TG32, KSEL(SEG_KCODE), vectrap, DPL_KERN);
	/*int printf(char*,...);
	printf("vectrap%p\n",vectrap);*/

  set_idt(idt, sizeof(idt));

  // register event handler
  user_handler = handler;

  return 0;
}

_Context *_kcontext(_Area stack, void (*entry)(void *), void *arg) {
  return NULL;
}

void _yield() {
  asm volatile("int $0x81");
}

int _intr_read() {
  return 0;
}

void _intr_write(int enable) {
}
