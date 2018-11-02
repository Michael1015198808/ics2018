#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);

  print_asm_template2(mov);
}

make_EHelper(push) {
	rtl_push(&id_dest->val);

  print_asm_template1(push);
}

make_EHelper(pop) {
	rtl_pop(&cpu.gpr[id_dest->reg]._32);


  print_asm_template1(pop);
}

make_EHelper(pusha) {
  TODO();

  print_asm("pusha");
}

make_EHelper(popa) {
  TODO();

  print_asm("popa");
}

make_EHelper(leave) {
	if(id_src->width==4){
					cpu.esp=cpu.ebp;
	}else{
					cpu.gpr[R_ESP]._16=cpu.gpr[R_EBP]._16;
	}
	rtl_pop(&cpu.ebp);

  print_asm("leave");
}

make_EHelper(cltd) {
  if (decoding.is_operand_size_16) {
					cpu.gpr[R_EDX]._16=cpu.gpr[R_EAX]._16&0x8000?0xffff:0;
  }
  else {
					cpu.gpr[R_EDX]._32=cpu.gpr[R_EAX]._32&0x80000000?0xffffffff:0;
  }

  print_asm(decoding.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decoding.is_operand_size_16) {
    TODO();
  }
  else {
    TODO();
  }

  print_asm(decoding.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  rtl_sext(&t0, &id_src->val, id_src->width);
  operand_write(id_dest, &t0);

  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);

  print_asm_template2(movzx);
}

make_EHelper(lea) {
	//printf("lea dest is %d\n",id_dest->reg);
  operand_write(id_dest, &id_src->addr);

  print_asm_template2(lea);
}
