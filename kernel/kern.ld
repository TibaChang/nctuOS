/* Link kernel as a ELF file .
See the GNU ld 'info' manual ("info ld") to learn the syntax. */

OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(_start)
kernel_load_addr = 0xF0100000;

SECTIONS
{
. = kernel_load_addr;

  /* AT(...) gives the load address of this section, which tells
  the boot loader where to load the kernel in physical memory */
. = ALIGN(0x1000);
PROVIDE(UTEXT_start = .);
PROVIDE(stext = .);
.text : AT(0x100000) {
  lib/string.o (.text)
  lib/printf.o (.text)
  lib/printfmt.o (.text)
  lib/readline.o (.text)
  lib/console.o (.text)
  lib/syscall.o (.text)
  user/shell.o (.text)
  user/main.o (.text)
  /usr/lib/gcc/i686-redhat-linux/4.5.1/libgcc.a (.*)
  PROVIDE(UTEXT_end = .);
  *(.text .stub .text.* .gnu.linkonce.t.*)
}
PROVIDE(etext = .);	/* Define the 'etext' symbol to this value */

. = ALIGN(0x1000);
PROVIDE(URODATA_start = .);
.rodata : {
  lib/string.o (.rodata)
  lib/printf.o (.rodata)
  lib/printfmt.o (.rodata)
  lib/readline.o (.rodata)
  lib/console.o (.rodata)
  lib/syscall.o (.rodata)
  user/shell.o (.rodata)
  user/main.o (.rodata)
PROVIDE(URODATA_end = .);
  *(.rodata .rodata.* .gnu.linkonce.r.*)
}
PROVIDE(rdata_end = .);

/* Include debugging information in kernel memory */
.stab : {
  PROVIDE(__STAB_BEGIN__ = .);
  *(.stab);
  PROVIDE(__STAB_END__ = .);
  BYTE(0)		/* Force the linker to allocate space
  for this section */
}

.stabstr : {
  PROVIDE(__STABSTR_BEGIN__ = .);
  *(.stabstr);
  PROVIDE(__STABSTR_END__ = .);
  BYTE(0)		/* Force the linker to allocate space
  for this section */
}

/* Adjust the address for the data segment to the next page */
. = ALIGN(0x1000);

/* The data segment */
PROVIDE(data_start = .);
.data : {
PROVIDE(UDATA_start = .);
  lib/string.o (.data)
  lib/printf.o (.data)
  lib/printfmt.o (.data)
  lib/readline.o (.data)
  lib/console.o (.data)
  lib/syscall.o (.data)
  user/shell.o (.data)
  user/main.o (.data)
PROVIDE(UDATA_end = .);
  *(.data)
}
PROVIDE(bss_start = .);
.bss : {
PROVIDE(UBSS_start = .);
  lib/string.o (.bss)
  lib/printf.o (.bss)
  lib/printfmt.o (.bss)
  lib/readline.o (.bss)
  lib/console.o (.bss)
  lib/syscall.o (.bss)
  user/shell.o (.bss)
  user/main.o (.bss)
  *(.bss)
}
PROVIDE(UBSS_end = .);
PROVIDE(end = .);

/DISCARD/ : {
  *(.eh_frame .note.GNU-stack)
}
}
