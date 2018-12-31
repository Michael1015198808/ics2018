#include "fs.h"

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
  size_t open_offset;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  {"stdin", 0, 0, invalid_read, invalid_write},
  {"stdout", 0, 0, invalid_read, invalid_write},
  {"stderr", 0, 0, invalid_read, invalid_write},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void init_fs() {
  // TODO: initialize the size of /dev/fb
}
int fs_open(const char *pathname, int flags, int mode){
    int i=0;
    for(i=0;i<NR_FILES;++i){
        if(!strcmp(file_table[i].name,pathname)){
            break;
        }
    }
    assert(i<NR_FILES);
    file_table[i].open_offset=0;
    return i;
}
size_t fs_read(int fd, void *buf, size_t len){
    if(file_table[fd].read==NULL){
        size_t RAMDISK_SIZE=get_ramdisk_size();
        if(file_table[fd].open_offset+file_table[fd].disk_offset+len>RAMDISK_SIZE){
            return -1;
            len=RAMDISK_SIZE-file_table[fd].open_offset-file_table[fd].disk_offset;
        }
        int ret=ramdisk_read(buf,file_table[fd].open_offset+file_table[fd].disk_offset,len);
        file_table[fd].open_offset+=len;
        return ret;
    }else{
        int ret=file_table[fd].read(buf,file_table[fd].open_offset+file_table[fd].disk_offset,len);
        file_table[fd].open_offset+=len;
        return ret;
    }
}
size_t fs_filesz(int fd){
    if(fd==NR_FILES-1){
        size_t RAMDISK_SIZE=get_ramdisk_size();
        return RAMDISK_SIZE;
    }else{
        return file_table[fd+1].disk_offset-file_table[fd].disk_offset;
    }
}
size_t fs_write(int fd, const void *buf, size_t len){
    if(fd==1||fd==2) {
        int i;
        for (i = 0; i < len; ++i) {
            _putc(*(char*)(buf + i));
        }
        return i;
    }else{
        int ret=file_table[fd].write(buf,file_table[fd].open_offset,len);
        file_table[fd].open_offset+=len;
        return ret;
    }
}
size_t fs_lseek(int fd, size_t offset, int whence){
    switch(whence){
        case SEEK_SET:
            return file_table[fd].open_offset=offset;
        case SEEK_CUR:
            return file_table[fd].open_offset+=offset;
        case SEEK_END:
            return file_table[fd].open_offset=fs_filesz(fd)-offset;
        default:
            assert(0);
    }
}
int fs_close(int fd){return file_table[fd].open_offset=0;}
