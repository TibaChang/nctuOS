#include <inc/mmu.h>
#include <inc/trap.h>

/* TRAPHANDLER defines a globally-visible function for handling a trap.
 * It pushes a trap number onto the stack, then jumps to _alltraps.
 * Use TRAPHANDLER for traps where the CPU automatically pushes an error code.
 *
 * You shouldn't call a TRAPHANDLER function from C, but you may
 * need to _declare_ one in C (for instance, to get a function pointer
 * during IDT setup).  You can declare the function with
 *   void NAME();
 * where NAME is the argument passed to TRAPHANDLER.
 */
#define TRAPHANDLER(name, num)						\
	.globl name;		/* define global symbol for 'name' */	\
	.type name, @function;	/* symbol type is function */		\
	.align 2;		/* align function definition */		\
	name:			/* function starts here */		\
	pushl $(num);							\
	jmp _alltraps

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

/* ISRs */
TRAPHANDLER_NOEC(Default_ISR, T_DEFAULT)
TRAPHANDLER_NOEC(KBD_Input, IRQ_OFFSET+IRQ_KBD)
TRAPHANDLER_NOEC(TIM_ISR, IRQ_OFFSET+IRQ_TIMER)

// TODO: Lab 5
// Please add interface of system call

TRAPHANDLER_NOEC(GPFLT, T_GPFLT)
TRAPHANDLER_NOEC(STACK_ISR, T_STACK)
TRAPHANDLER_NOEC(PGFLT, T_PGFLT)

.globl default_trap_handler;
_alltraps:
	/* Lab3: Push the registers into stack( fill the Trapframe structure )
	 * You can reference the http://www.osdever.net/bkerndev/Docs/isrs.htm
	 * After stack parpared, just "call default_trap_handler".
	 */
	pushl %ds
	pushl %es
	pushal # Push all general register into stack, it maps to Trapframe.tf_regs
	/* Load the Kernel Data Segment descriptor */
	mov $(GD_KD), %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	
	pushl %esp # Pass a pointer to the Trapframe as an argument to default_trap_handler()
	call default_trap_handler
	
	/* Restore fs and gs to user data segmemnt */
	push %ax
	mov $(GD_UD), %ax
	or $3, %ax
	mov %ax, %fs
	mov %ax, %gs
	pop %ax 
	add $4, %esp

# Return falls through to trapret...
.globl trapret
trapret:
  popal
  popl %es
  popl %ds
  addl $0x8, %esp  # trapno and errcode
  iret

