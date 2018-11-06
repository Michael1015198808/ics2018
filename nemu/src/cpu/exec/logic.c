#include "cpu/exec.h"
#include "cpu/cc.h"

make_EHelper(test) {
	//printf("cl%d\n",reg_b(R_CL));
	//printf("%d\t%d\ntest\n",id_dest->val,id_src->val);
	rtl_and(&t2,&id_dest->val,&id_src->val);
	cpu.OF=cpu.CF=0;
	rtl_update_ZFSF(&t2,id_dest->width);

  print_asm_template2(test);
}

make_EHelper(and) {
	rtl_and(&t2,&id_dest->val,&id_src->val);
	cpu.OF=cpu.CF=0;
	rtl_update_ZFSF(&t2,id_dest->width);
	operand_write(id_dest,&t2);

  print_asm_template2(and);
}

make_EHelper(xor) {
	rtl_xor(&t2,&id_dest->val,&id_src->val);
	cpu.OF=cpu.CF=0;
	rtl_update_ZFSF(&t2,id_dest->width);
	operand_write(id_dest,&t2);

  print_asm_template2(xor);
}

make_EHelper(or) {
	rtl_or(&t2,&id_dest->val,&id_src->val);
	cpu.OF=cpu.CF=0;
	rtl_update_ZFSF(&t2,id_dest->width);
	operand_write(id_dest,&t2);

  print_asm_template2(or);
}

make_EHelper(sar) {
	if(id_dest->width==2){
					id_dest->val=(int32_t)(int16_t)id_dest->val;
	}
	rtl_sar(&t0,&id_dest->val,&id_src->val);
	rtl_update_ZFSF(&t0,id_dest->width);
	operand_write(id_dest,&t0);
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(sar);
}

make_EHelper(shl) {
	rtl_shl(&t0,&id_dest->val,&id_src->val);
	rtl_update_ZFSF(&t0,id_dest->width);
	operand_write(id_dest,&t0);
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shl);
}

make_EHelper(shr) {
	rtl_shr(&t0,&id_dest->val,&id_src->val);
	rtl_update_ZFSF(&t0,id_dest->width);
	operand_write(id_dest,&t0);
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shr);
}

make_EHelper(setcc) {
  uint32_t cc = decoding.opcode & 0xf;

  rtl_setcc(&t2, cc);
	//printf("%d",id_dest->width);
  operand_write(id_dest, &t2);

  print_asm("set%s %s", get_cc_name(cc), id_dest->str);
}

make_EHelper(not) {
	rtl_not(&t0,&id_dest->val);
	rtl_update_ZFSF(&t0,id_dest->width);
	operand_write(id_dest,&t0);

  print_asm_template1(not);
}

make_EHelper(rol) {
	rtl_shl(&t0,&id_dest->val,&id_src->val);
	t0|=(id_dest->val)>>(id_dest->width*8-id_src->val);
	operand_write(id_dest,&t0);
}

make_EHelper(ror) {
	rtl_shr(&t0,&id_dest->val,&id_src->val);
	t0|=(id_dest->val)<<(id_dest->width*8-id_src->val);
	operand_write(id_dest,&t0);
}
