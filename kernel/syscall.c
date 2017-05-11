#include <kernel/task.h>
#include <kernel/timer.h>
#include <kernel/mem.h>
#include <kernel/cpu.h>
#include <kernel/syscall.h>
#include <kernel/trap.h>
#include <inc/stdio.h>

extern void sys_settextcolor(unsigned char forecolor, unsigned char backcolor); // kernel/screen.c
extern void sys_cls(); // kernel/screen.c
extern void sched_yield();

volatile struct spinlock PT_LOCK;
void do_puts(char *str, uint32_t len)
{
	uint32_t i;
	spin_lock(&PT_LOCK);
	for (i = 0; i < len; i++)
	{
		k_putch(str[i]);
	}
	spin_unlock(&PT_LOCK);
}

int32_t do_getc()
{
	return k_getc();
}


int32_t do_syscall(uint32_t syscallno, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5)
{
	int32_t retVal = -1;

	switch (syscallno)
	{
	case SYS_fork:
		/* TODO: Lab 5
     	* You can reference kernel/task.c, kernel/task.h
     	*/
		retVal = sys_fork(); //In task.c
		break;

	case SYS_getc:
		retVal = do_getc();
		break;

	case SYS_puts:
		do_puts((char*)a1, a2);
		retVal = 0;
		break;

	case SYS_getpid:
		/* TODO: Lab 5
     	* Get current task's pid
     	*/
		retVal = thiscpu->cpu_task->task_id;
		break;

	case SYS_getcid:
		/* Lab6: get current cpu's cid */
		retVal = thiscpu->cpu_id;
		break;

	case SYS_sleep:
		/* TODO: Lab 5
     	* Yield this task
     	* You can reference kernel/sched.c for yielding the task
     	*/
        if (thiscpu->cpu_task != NULL)
        {   
            thiscpu->cpu_task->remind_ticks = a1; 
            thiscpu->cpu_task->state = TASK_SLEEP;
            sched_yield();
        }   
        retVal = 0;
		break;

	case SYS_kill:
		/* TODO: Lab 5
     	* Kill specific task
     	* You can reference kernel/task.c, kernel/task.h
     	*/
		sys_kill(a1); //In task.c
		retVal = 0;
		break;

  case SYS_get_num_free_page:
		/* TODO: Lab 5
     	* You can reference kernel/mem.c
     	*/
		retVal = sys_get_num_free_page();
    	break;

  case SYS_get_num_used_page:
		/* TODO: Lab 5
     	* You can reference kernel/mem.c
     	*/
		retVal = sys_get_num_used_page();
    	break;

  case SYS_get_ticks:
		/* TODO: Lab 5
     	* You can reference kernel/timer.c
     	*/
    	retVal = sys_get_ticks();
    	break;

  case SYS_settextcolor:
		/* TODO: Lab 5
     	* You can reference kernel/screen.c
     	*/
		sys_settextcolor(a1,a2);
		retVal = 0;
    	break;

  case SYS_cls:
		/* TODO: Lab 5
     	* You can reference kernel/screen.c
     	*/
		sys_cls();
		retVal = 0;
    	break;
  /* TODO: Lab7 file I/O system call */
  case SYS_open:
  		retVal = sys_open(a1, a2, a3);
		break;
  case SYS_read:
  		retVal = sys_read(a1, a2, a3);
		break;
  case SYS_write:
  		retVal = sys_write(a1, a2, a3);
		break;
  case SYS_close:
  		retVal = sys_close(a1);
		break;
  case SYS_lseek:
  		retVal = sys_lseek(a1, a2, a3);
		break;
  case SYS_unlink:
		retVal = sys_unlink(a1);
    	break;
  }
	return retVal;
}

static void syscall_handler(struct Trapframe *tf)
{
	/* TODO: Lab5
   * call do_syscall
   * Please remember to fill in the return value
   * HINT: You have to know where to put the return value
   */
    int32_t ret = -1; 
    ret = do_syscall(tf->tf_regs.reg_eax, tf->tf_regs.reg_edx, tf->tf_regs.reg_ecx, tf->tf_regs.reg_ebx, tf->tf_regs.reg_edi, tf->tf_regs.reg_esi); 
    /* Set system return value */
    tf->tf_regs.reg_eax = ret;
}

void syscall_init()
{
  /* TODO: Lab5
   * Please set gate of system call into IDT
   * You can leverage the API register_handler in kernel/trap.c
   */
    extern void SYS_ISR();
    register_handler( T_SYSCALL, &syscall_handler, &SYS_ISR, 1, 3);//ring 3 can call this handler
}

