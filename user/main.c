#include <inc/stdio.h>
#include <inc/syscall.h>
#include <inc/shell.h>

int user_entry()
{

	asm volatile("movl %0,%%eax\n\t" \
    "movw %%ax,%%ds\n\t" \
    "movw %%ax,%%es\n\t" \
    "movw %%ax,%%fs\n\t" \
    "movw %%ax,%%gs" \
    :: "i" (0x20 | 0x03)
  );

  cprintf("Welcome to User Land, cheers!\n");
  shell();
  for(;;){};
}
