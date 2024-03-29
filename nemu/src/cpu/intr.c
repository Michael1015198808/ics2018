#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */

	rtl_push(&cpu.eflags);
	rtl_push(&cpu.CS);
	rtl_push(&ret_addr);

	rtl_j(
		(vaddr_read(cpu.IDTR.offset+(NO<<3)+4,4)&0xffff0000)+
		(vaddr_read(cpu.IDTR.offset+(NO<<3)  ,4)&0x0000ffff));
	return;
}

void dev_raise_intr() {
}
