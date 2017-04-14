#include <inc/string.h>
#include <inc/syscall.h>


void
cputchar(int ch)
{
	char c = ch;

	// Unlike standard Unix's putchar,
	// the cputchar function _always_ outputs to the system console.
	puts(&c, 1);
}

int
getchar(void)
{
	int r;
	// sys_cgetc does not block, but getchar should.
	while ((r = getc()) == 0){};
		//sys_yield();
	return r;
}
