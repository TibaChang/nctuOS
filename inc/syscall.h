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
#endif
