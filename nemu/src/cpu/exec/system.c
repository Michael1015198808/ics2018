#include "cpu/exec.h"

void difftest_skip_ref();
void difftest_skip_dut();

make_EHelper(lidt) {
	cpu.IDTR=id_dest->val;

  print_asm_template1(lidt);
}

make_EHelper(mov_r2cr) {
  TODO();

  print_asm("movl %%%s,%%cr%d", reg_name(id_src->reg, 4), id_dest->reg);
}

make_EHelper(mov_cr2r) {
  TODO();

  print_asm("movl %%cr%d,%%%s", id_src->reg, reg_name(id_dest->reg, 4));

#if defined(DIFF_TEST)
  difftest_skip_ref();
#endif
}

make_EHelper(int) {
void raise_intr(uint8_t,vaddr_t);
	raise_intr(id_dest->val,decoding.seq_eip);

  print_asm("int %s", id_dest->str);

#if defined(DIFF_TEST) && defined(DIFF_TEST_QEMU)
  difftest_skip_dut();
#endif
}

make_EHelper(iret) {
    /*static int i=0;
    while(++i<2){
        Log("iret\n");
    }*/

	rtl_pop(&decoding.seq_eip);
	rtl_pop(&cpu.CS);
	rtl_pop(&cpu.eflags);

  print_asm("iret");
}

make_EHelper(in) {
	uint32_t pio_read_b(ioaddr_t addr);
	uint32_t pio_read_w(ioaddr_t addr);
	uint32_t pio_read_l(ioaddr_t addr);
	//printf("width%d\n",id_dest->width);
	switch(id_dest->width){
		case 1:
			at=pio_read_b(id_src->val);
			break;
		case 2:
			at=pio_read_w(id_src->val);
			break;
		case 4:
			at=pio_read_l(id_src->val);
			break;
		default:
			printf("Something wrong in EHelper(in)\n");
	}
	operand_write(id_dest,&at);

  print_asm_template2(in);

#if defined(DIFF_TEST)
  difftest_skip_ref();
#endif
}


make_EHelper(out) {
	void pio_write_b(ioaddr_t addr, uint32_t data);
	pio_write_b(id_dest->val,id_src->val);

  print_asm_template2(out);

#if defined(DIFF_TEST)
  difftest_skip_ref();
#endif
}
