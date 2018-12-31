#include "common.h"
#include <amdev.h>

static int width=0;
static int height=0;
size_t serial_write(const void *buf, size_t offset, size_t len) {
  int i;
  for(i=0;i<len;++i){
    _putc(*(char*)(buf+i));
  }
  return i;
}

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t offset, size_t len) {
  return 0;
}

static char dispinfo[128] __attribute__((used));

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  if(len+offset>120){
    len=120-offset;
  }
  memcpy(buf, dispinfo + offset, len);
  return len;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  printf("Write\n");
  int i, x, y;
  offset>>=2;
  y = offset / width;
  x = offset - y * width;
  for (i = 0; i < len; ++i) {
    draw_rect(((uint32_t*)buf), x, y, 1, 1);
    ++x;
    if (x==width){x=0;++y;if(y==height){y=0;}}
  }
  return 0;
}

void init_device() {
  Log("Initializing devices...");
  _ioe_init();
  sprintf(dispinfo,"WIDTH:%d\nHEIGHT:%d\n",width=screen_width(),height=screen_height());

  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
}
