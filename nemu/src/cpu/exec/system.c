#include "cpu/exec.h"

void difftest_skip_ref();
void difftest_skip_dut();

make_EHelper(lidt) {
	cpu.IDTR.size= vaddr_read(id_dest->addr,2);
	cpu.IDTR.offset= vaddr_read(id_dest->addr+2,4);

  print_asm_template1(lidt);
}

make_EHelper(mov_r2cr) {
  operand_write(id_dest, &id_src->val);

  print_asm("movl %%%s,%%cr%d", reg_name(id_src->reg, 4), id_dest->reg);
}

make_EHelper(mov_cr2r) {
  operand_write(id_dest, &id_src->val);

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

	rtl_pop(&at);
    rtl_j(at);
	rtl_pop(&cpu.CS);
	rtl_pop(&cpu.eflags.val);

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

make_EHelper(cheat){
  Log("cheat");
  uintptr_t s=paddr_read(cpu.esp+4,4);
  int c;
    do{
      putchar(c=vaddr_read(s++,1));
    }while(c!='\0');
  printf("0x%08x\n",paddr_read(cpu.esp+8,4));
  print_asm( "nemu cheat");
}
make_EHelper(cheat2){
  char input[25],output[25];
  int c,idx=0;
  uintptr_t s=paddr_read(cpu.esp+4,4);
  do{
    input[idx++]=(c=vaddr_read(s++,1));
  }while(c!='\0');
  puts(input);
  printf("%d\n",sscanf(input,"kd %s",output));
  print_asm( "nemu cheat2");
}
