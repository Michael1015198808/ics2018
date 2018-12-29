#include "cpu/exec.h"
#include "cpu/cc.h"

make_EHelper(jmp) {
  // the target address is calculated at the decode stage
  rtl_j(decoding.jmp_eip);

  print_asm("jmp %x", decoding.jmp_eip);
}

make_EHelper(jcc) {
  // the target address is calculated at the decode stage
  uint32_t cc = decoding.opcode & 0xf;
  rtl_setcc(&t0, cc);
  rtl_li(&t1, 0);
	//printf("t0:%d\tt1:%d\n",t0,t1);
  rtl_jrelop(RELOP_NE, &t0, &t1, decoding.jmp_eip);

  print_asm("j%s %x", get_cc_name(cc), decoding.jmp_eip);
}

make_EHelper(jmp_rm) {
  rtl_jr(&id_dest->val);

  print_asm("jmp *%s", id_dest->str);
}

make_EHelper(call) {
  // the target address is calculated at the decode stage
	rtl_push(eip);
  rtl_j(decoding.jmp_eip);

  print_asm("call %x", decoding.jmp_eip);
}

make_EHelper(ret) {
    Log("cpu.eip%x\ndecoding.seq_eip%x\nreturn\n",cpu.eip,decoding.seq_eip);
	rtl_pop(eip);
    Log("cpu.eip%x\ndecoding.seq_eip%x\nreturn\n",cpu.eip,decoding.seq_eip);

  print_asm("ret");
}

make_EHelper(call_rm) {
	rtl_push(eip);
  rtl_jr(&id_dest->val);

  print_asm("call *%s", id_dest->str);
}
