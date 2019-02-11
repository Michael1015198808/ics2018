#include "common.h"
_Context* do_syscall(_Context*);
static _Context *do_event(_Event e, _Context *c) {
    switch (e.event) {
        case _EVENT_YIELD:
            //printf("一句话即可\n");
            //Set in PA3.1
            return schedule(c);
            break;
        case _EVENT_SYSCALL:
            //Log("syscall%d,%d,%x,%d",c->GPR1,c->GPR2,c->GPR3,c->GPR4);
            do_syscall(c);
            break;
        default:
            panic("Unhandled event ID = %d", e.event);
    }

    return NULL;
}

void init_irq(void) {
    Log("Initializing interrupt/exception handler...");
    _cte_init(do_event);
}
