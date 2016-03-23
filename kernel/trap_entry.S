#include <inc/mmu.h>
#include <inc/trap.h>

/* Use TRAPHANDLER_NOEC for traps where the CPU doesn't push an error code.
 * It pushes a 0 in place of the error code, so the trap frame has the same
 * format in either case.
 */
#define TRAPHANDLER_NOEC(name, num)					\
	.globl name;							\
	.type name, @function;						\
	.align 2;							\
	name:								\
	pushl $0;							\
	pushl $(num);							\
	jmp _alltraps


.text

/* TODO: Interface declaration for ISRs
 * Note: Use TRAPHANDLER_NOEC macro define other isr enrty
 *       The Trap number are declared in inc/trap.h which might come in handy
 *       when declaring interface for ISRs.
 */

.globl default_trap_handler;
_alltraps:
  /* TODO: Since we are going to call default_trap_handler, we have to prepare
   *       the argument for it, which is placed on the stack.
   *       Thus push registers to fillup the Trapframe data structure.
   * 
   * Note: The x86 CPU will automatically save some register when interrupts
   *       occurs. Thus, you do not have to push those registers.
   *       Please reference lab3.docx for what registers are pushed by
   *       CPU.
   *       You may want to leverage the "pusha" instructions to reduce your work of
   *       pushing all the general purpose registers into the stack.
	 */

	pushl %esp # Pass a pointer which points to the Trapframe as an argument to default_trap_handler()
	call default_trap_handler

	add $8, %esp # Cleans up the pushed error code and pushed ISR number
	iret # pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP!

