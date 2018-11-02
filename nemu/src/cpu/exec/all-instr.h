#include "cpu/exec.h"

make_EHelper(mov);

make_EHelper(operand_size);

make_EHelper(inv);
make_EHelper(nemu_trap);
//Before PA2
make_EHelper(call);
make_EHelper(push);
make_EHelper(sub);
make_EHelper(xor);
make_EHelper(pop);
make_EHelper(ret);
//For dummy.c
make_EHelper(lea);
make_EHelper(and);
make_EHelper(add);
make_EHelper(inc);
make_EHelper(cmp);
make_EHelper(jcc);
make_EHelper(setcc);
make_EHelper(movzx);
make_EHelper(test);
make_EHelper(leave);
//For sum.c
make_EHelper(dec);
//For unalign.c
make_EHelper(nop);
make_EHelper(cltd);
make_EHelper(idiv);
//For wanshu.c
//Nothing for mov-c.c
make_EHelper(jmp);
make_EHelper(imul2);
//For fact.c
//Nothing for div.c
make_EHelper(mul);
make_EHelper(shr);
//For shuixianhua.c
//Nothing for prime.c, goldbach.c, fib.c, switch.c
//



