#ifndef USR_SYSCALL_H
#define USR_SYSCALL_H
#include <inc/types.h>

/* system call numbers */
enum {
  SYS_puts = 0,
  SYS_getc,
  SYS_getpid,
  SYS_getcid,
  SYS_fork,
  SYS_kill,
  SYS_sleep,
  SYS_get_num_used_page,
  SYS_get_num_free_page,
  SYS_get_ticks,
  SYS_settextcolor,
  SYS_cls,
  SYS_open,
  SYS_close,
  SYS_read,
  SYS_write,
  SYS_lseek,
  SYS_unlink,
  SYS_readdir,
  NSYSCALLS
};

int32_t get_num_used_page(void);

int32_t cls(void);

int32_t get_num_free_page(void);

unsigned long get_ticks(void);

void settextcolor(unsigned char forecolor, unsigned char backcolor);

int32_t fork(void);

int32_t getpid(void);

int32_t getcid(void);

void kill_self();

void sleep(uint32_t ticks);

void puts(const char *s, size_t len);
int getc(void);

/*********** Lab7 ************/
int sys_open(const char *file, int flags, int mode);
int sys_close(int d);
int sys_read(int fd, void *buf, size_t len);
int sys_write(int fd, const void *buf, size_t len);
off_t sys_lseek(int fd, off_t offset, int whence);
int sys_unlink(const char *pathname);
int sys_readdir(int fd,  char *buf ,int *type, unsigned long *size);
#endif