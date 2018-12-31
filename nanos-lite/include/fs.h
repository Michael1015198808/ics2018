#ifndef __FS_H__
#define __FS_H__

#include "common.h"

#ifndef SEEK_SET
enum {SEEK_SET, SEEK_CUR, SEEK_END};
#endif
size_t get_ramdisk_size(void);
//writing functions
size_t ramdisk_write(void*, int, size_t);
size_t serial_write(const void*,size_t,size_t);
size_t fb_write(const void*,size_t,size_t);

//reading functions
size_t ramdisk_read(void*, size_t, size_t);
size_t dispinfo_read(void*, size_t, size_t);
#endif
