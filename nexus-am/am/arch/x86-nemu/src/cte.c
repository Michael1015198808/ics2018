#include <am.h>
#include <x86.h>

static _Context *(*user_handler)(_Event, _Context *) = NULL;

void vectrap();
void vecsys();
void vecnull();
extern _Protect *cur_as;
_Context *irq_handle(_Context *tf) {
    void get_cur_as(_Context*);
    void test(void);
    const char code[]={0xf1,0xc3};
    ((void(*)(int,uintptr_t))code)(1,(uintptr_t)cur_as);
    if(cur_as!=NULL)((void(*)(int,uintptr_t))code)(1,(uintptr_t)cur_as->ptr);
    get_cur_as(tf);
    ((void(*)(int,uintptr_t))code)(1,(uintptr_t)tf->prot);
    //test();
    _Context *next = tf;
    //while(next==NULL);
    if (user_handler) {
        _Event ev={0};
        switch (tf->irq) {
            case 0x81:
                ev.event = _EVENT_YIELD;
                break;
            case 0x80:
                ev.event = _EVENT_SYSCALL;
                break;
            default:
                ev.event = _EVENT_ERROR;
                break;
        }

        next = user_handler(ev, tf);
        if (next == NULL) {
            next = tf;
        }
    }


    void _switch(_Context*);
    //test();
    _switch(next);
    //test();
    return next;
}

static GateDesc idt[NR_IRQ];

int _cte_init(_Context *(*handler)(_Event, _Context *)) {
    // initialize IDT
    for (unsigned int i = 0; i < NR_IRQ; i++) {
        idt[i] = GATE(STS_TG32, KSEL(SEG_KCODE), vecnull, DPL_KERN);
    }

    // -------------------- system call --------------------------
    idt[0x81] = GATE(STS_TG32, KSEL(SEG_KCODE), vectrap, DPL_KERN);
    idt[0x80] = GATE(STS_TG32, KSEL(SEG_KCODE), vecsys, DPL_KERN);

    set_idt(idt, sizeof(idt));

    // register event handler
    user_handler = handler;

    return 0;
}

_Context *_kcontext(_Area stack, void (*entry)(void *), void *arg) {
    _Context *c = (_Context*)(stack.end) - 1;

    *(_Context**)(stack.start)=c;
    c->cs=8;
    c->eip=(uintptr_t)entry;
    return c;
}

void _yield() {
    asm volatile("int $0x81");
}

int _intr_read() {
    return 0;
}

void _intr_write(int enable) {
}
