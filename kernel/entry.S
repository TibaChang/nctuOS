#include <inc/mmu.h>
#include <inc/memlayout.h>

#define RELOC(x) ((x) - KERNBASE)

.globl _start
.global entry
_start = RELOC(entry)

.text
entry:
	movw	$0x1234,0x472			# warm boot

	# We haven't set up virtual memory yet, so we're running from
	# the physical address the boot loader loaded the kernel at: 1MB
	# (plus a few bytes).  However, the C code is linked to run at
	# KERNBASE+1MB.  Hence, we set up a trivial page directory that
	# translates virtual addresses [KERNBASE, KERNBASE+4MB) to
	# physical addresses [0, 4MB).  This 4MB region will be
	# sufficient until we set up our real page table in mem_init.

	# Load the physical address of entry_pgdir into cr3.  entry_pgdir
	# is defined in entrypgdir.c.
	movl	$(RELOC(entry_pgdir)), %eax
	movl	%eax, %cr3
	# Turn on paging.
	movl	%cr0, %eax
	orl	$(CR0_PE|CR0_PG|CR0_WP), %eax
	movl	%eax, %cr0

	# Now paging is enabled, but we're still running at a low EIP
	# (why is this okay?).  Jump up above KERNBASE before entering
	# C code.
	mov	$relocated, %eax
	jmp	*%eax

relocated:

    # Setup new gdt
    lgdt    kgdtdesc

	# Setup kernel stack
	movl $0, %ebp
	movl $(bootstacktop), %esp

	call kernel_main
die:
	jmp die

.p2align 2                                # force 4 byte alignment
kgdt:
    SEG_NULL    # NULL segment
    SEG(STA_X|STA_R, 0x0, 0xffffffff)   # Kernel code segment
    SEG(STA_W, 0x0, 0xffffffff)         # Kernel data segment

kgdtdesc:
    .word   0x17        # sizeof(gdt) - 1
    .long   kgdt

.bss
	# There is kernel initial stack
	.globl		bootstack
bootstack:
	.space		KSTKSIZE
	.globl		bootstacktop   
bootstacktop:

