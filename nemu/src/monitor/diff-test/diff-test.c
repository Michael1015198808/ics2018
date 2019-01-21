#include <dlfcn.h>

#include "nemu.h"
#include "monitor/monitor.h"
#include "diff-test.h"

static void (*ref_difftest_memcpy_from_dut)(paddr_t dest, void *src, size_t n);
static void (*ref_difftest_getregs)(void *c);
static void (*ref_difftest_setregs)(const void *c);
static void (*ref_difftest_exec)(uint64_t n);

static bool is_skip_ref;
static bool is_skip_dut;

void difftest_skip_ref() { is_skip_ref = true; }
void difftest_skip_dut() { is_skip_dut = true; }

void init_difftest(char *ref_so_file, long img_size) {
#ifndef DIFF_TEST
  return;
#endif

  assert(ref_so_file != NULL);

  void *handle;
  handle = dlopen(ref_so_file, RTLD_LAZY | RTLD_DEEPBIND);
  assert(handle);

  ref_difftest_memcpy_from_dut = dlsym(handle, "difftest_memcpy_from_dut");
  assert(ref_difftest_memcpy_from_dut);

  ref_difftest_getregs = dlsym(handle, "difftest_getregs");
  assert(ref_difftest_getregs);

  ref_difftest_setregs = dlsym(handle, "difftest_setregs");
  assert(ref_difftest_setregs);

  ref_difftest_exec = dlsym(handle, "difftest_exec");
  assert(ref_difftest_exec);

  void (*ref_difftest_init)(void) = dlsym(handle, "difftest_init");
  assert(ref_difftest_init);

  Log("Differential testing: \33[1;32m%s\33[0m", "ON");
  Log("The result of every instruction will be compared with %s. "
      "This will help you a lot for debugging, but also significantly reduce the performance. "
      "If it is not necessary, you can turn it off in include/common.h.", ref_so_file);

  ref_difftest_init();
  ref_difftest_memcpy_from_dut(ENTRY_START, guest_to_host(ENTRY_START), img_size);
  ref_difftest_setregs(&cpu);
}

void difftest_step(uint32_t eip) {
  CPU_state ref_r;

  if (is_skip_dut) {
    is_skip_dut = false;
    return;
  }

  if (is_skip_ref) {
    // to skip the checking of an instruction, just copy the reg state to reference design
    ref_difftest_setregs(&cpu);
    is_skip_ref = false;
    return;
  }

  ref_difftest_exec(1);
  ref_difftest_getregs(&ref_r);

  // TODO: Check the registers state with the reference design.
  // Set `nemu_state` to `NEMU_ABORT` if they are not the same.
	int i;
	for(i=0;i<8;++i){
					if(cpu.gpr[i]._32!=ref_r.gpr[i]._32){
									printf("%s should be %4d\t(0x%x)\n",regsl[i],ref_r.gpr[i]._32,ref_r.gpr[i]._32);
									printf("   instead of %4d\t(0x%x)\n",cpu.gpr[i]._32,cpu.gpr[i]._32);
									nemu_state=NEMU_ABORT;
					}
	}
	if(cpu.eip!=ref_r.eip){
					printf("eip should be %4d\t(0x%x)\n",ref_r.eip,ref_r.eip);
					printf("   instead of %4d\t(0x%x)\n",cpu.eip,cpu.eip);
					nemu_state=NEMU_ABORT;
	}
#define check(f) \
	if(cpu.f!=ref_r.f){\
					printf("%s should be %d\n",#f,ref_r.f);\
					printf("  instead of %d\n",cpu.f);\
	}
	//check(CF);check(ZF);check(SF);check(OF);
    ref_difftest_setregs(&cpu);
}
void difftest_attach(void) {
#ifndef DIFF_TEST
  return;
#else
    //ref_difftest_init();
    ref_difftest_memcpy_from_dut(0, guest_to_host(0), 0x7c00);
#define PMEM_SIZE (128 * 1024 * 1024)
    ref_difftest_memcpy_from_dut(0x100000, guest_to_host(0x100000), PMEM_SIZE-0x100000);
    //ref_difftest_memcpy_from_dut(0x40991a8, guest_to_host(0x40991a8), 4);
#undef PMEM_SIZE
    char _lidt[]={0x0f,0x01,0x18};//The instructions to set idt
    ref_difftest_memcpy_from_dut(0x7e00, &cpu.IDTR.size, 2);
    ref_difftest_memcpy_from_dut(0x7e02, &cpu.IDTR.offset, 4);
    //Use two sentences to avoid 对齐

    /*int test;

    test=0x7f7f7f7f;
    printf("%x\n",test);
    ref_difftest_memcpy_from_dut(0x7e02, &test , 4);
    printf("%x\n",test);

    test=0x3f3f3f3f;
    printf("%x\n",test);
    ref_difftest_memcpy_from_dut((uintptr_t)&test ,(void*)0x7e02, 4);
    printf("%x\n",test);*/
    ref_difftest_memcpy_from_dut(0x7e40, _lidt, sizeof(_lidt));
    uint32_t old_eip=cpu.eip,old_eax=cpu.eax;
    cpu.eip=0x7e40;cpu.eax=0x7e00;
    ref_difftest_setregs(&cpu);
    ref_difftest_exec(1);
    cpu.eip=old_eip;cpu.eax=old_eax;
    ref_difftest_setregs(&cpu);
#endif
}

