#ifndef __SYSCALL_H__
#define __SYSCALL_H__

enum {
  SYS_exit,//0
  SYS_yield,//1
  SYS_open,//2
  SYS_read,//3
  SYS_write,//4
  SYS_kill,//5
  SYS_getpid,//6
  SYS_close,//7
  SYS_lseek,//8
  SYS_brk,//9
  SYS_fstat,//10
  SYS_time,
  SYS_signal,
  SYS_execve,
  SYS_fork,
  SYS_link,
  SYS_unlink,
  SYS_wait,
  SYS_times,
  SYS_gettimeofday
};
size_t fs_open(const char*,int,int);
size_t fs_read(int,void*,int);
size_t fs_write(int,void*,int);
int fs_close(int);
size_t fs_filesz(int);
size_t fs_lseek(int,size_t,int);
int mm_brk(uintptr_t);
#endif
