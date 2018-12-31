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
        {"stdout", 0, 0, invalid_read, serial_write},
        {"stderr", 0, 0, invalid_read, serial_write},
        {"/dev/fb", 0, 0, invalid_read, fb_write},
        {"/proc/dispinfo", 0, 0, dispinfo_read, invalid_write},

#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void init_fs() {
    assert(!strcmp(file_table[3].name,"/dev/fb"));
    int screen_height();
    int screen_width();
    file_table[3].size=screen_height()*screen_width()*(32/8);//RFB+Alpha=32bits
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
    Log("File %s no.%d\n",file_table[i].name,i);
    file_table[i].open_offset=0;
    return i;
}
size_t fs_read(int fd, void *buf, size_t len) {
    ReadFn read = file_table[fd].read == NULL ? (ReadFn)ramdisk_read : file_table[fd].read;
    if (file_table[fd].disk_offset + len > file_table[fd].size) {
        len = file_table[fd].size- file_table[fd].open_offset;
    }
    int ret = read(buf, file_table[fd].open_offset + file_table[fd].disk_offset, len);
    file_table[fd].open_offset += len;
    return ret;
}
size_t fs_filesz(int fd){
    return file_table[fd].size;
}
size_t fs_write(int fd, const void *buf, size_t len) {
    WriteFn write = file_table[fd].write == NULL ? (WriteFn) ramdisk_write : file_table[fd].write;
    if (file_table[fd].open_offset + len > file_table[fd].size) {
        len = file_table[fd].size - file_table[fd].open_offset;
    }
    int ret = write(buf, file_table[fd].open_offset + file_table[fd].disk_offset, len);
    file_table[fd].open_offset += len;
    return ret;
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
int fs_close(int fd){return 0;}
