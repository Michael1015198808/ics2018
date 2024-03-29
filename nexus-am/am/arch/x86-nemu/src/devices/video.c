#include <am.h>
#include <x86.h>
#include <amdev.h>
#include <klib.h>

static uint32_t* const fb __attribute__((used)) = (uint32_t *)0x40000;
#define SCREEN_PORT 0x100

size_t video_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_VIDEO_INFO: {
      _VideoInfoReg *info = (_VideoInfoReg *)buf;

      info->width=inl(SCREEN_PORT);
      info->height=info->width&0xffff;
      info->width=info->width>>16;
      //Learn from Han Bo
      //Great thanks to him!

      return sizeof(_VideoInfoReg);
    }
  }
  return 0;
}

size_t video_write(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_VIDEO_FBCTL: {
      _FBCtlReg *ctl = (_FBCtlReg *)buf;
			int x = ctl->x, y = ctl->y, w = ctl->w, h = ctl->h;
			uint32_t *pixels = ctl->pixels;
			for (int j = 0; j < h ; j ++) {
				memcpy(&fb[(y + j) * 400 + x], pixels, w*sizeof(uint32_t));
				pixels += w;
			}

      if (ctl->sync) {
        // do nothing, hardware syncs.
      }
      return sizeof(_FBCtlReg);
    }
  }
  return 0;
}

void vga_init() {
}

