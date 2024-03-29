#include <am.h>
#include <x86.h>
#include <amdev.h>

#define I8042_DATA_PORT 0x60

size_t input_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_INPUT_KBD: {
      _KbdReg *kbd = (_KbdReg *)buf;
			uint16_t i=inw(I8042_DATA_PORT);
      kbd->keydown = i&0x8000?1:0;
      kbd->keycode = i&0x7fff;
      return sizeof(_KbdReg);
    }
  }
  return 0;
}
/*
size_t input_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_INPUT_KBD: {
      _KbdReg *kbd = (_KbdReg *)buf;
      kbd->keydown = 0;
      kbd->keycode = _KEY_NONE;
      return sizeof(_KbdReg);
    }
  }
  return 0;
}
*/
