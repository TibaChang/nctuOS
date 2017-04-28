#include <kernel/task.h>
#include <kernel/cpu.h>
#include <inc/x86.h>
#include <inc/stdio.h>

#define ctx_switch(ts) \
  do { env_pop_tf(&((ts)->tf)); } while(0)

/* TODO: Lab5
* Implement a simple round-robin scheduler (Start with the next one)
*
* 1. You have to remember the task you picked last time.
*
* 2. If the next task is in TASK_RUNNABLE state, choose
*    it.
*
* 3. After your choice set cur_task to the picked task
*    and set its state, remind_ticks, and change page
*    directory to its pgdir.
*
* 4. CONTEXT SWITCH, leverage the macro ctx_switch(ts)
*    Please make sure you understand the mechanism.
*/
volatile static int32_t last_task_pid = 0;

//
// TODO: Lab6
// Modify your Round-robin scheduler to fit the multi-core
// You should:
//
// 1. Design your Runqueue structure first (in kernel/task.c)
//
// 2. modify sys_fork() in kernel/task.c ( we dispatch a task
//    to cpu runqueue only when we call fork system call )
//
// 3. modify sys_kill() in kernel/task.c ( we remove a task
//    from cpu runqueue when we call kill_self system call
//
// 4. modify your scheduler so that each cpu will do scheduling
//    with its runqueue
//    
//    (cpu can only schedule tasks which in its runqueue!!) 
//    (do not schedule idle task if there are still another process can run)	
//
void sched_yield(void)
{
	extern Task tasks[];
	last_task_pid = bootcpu->cpu_task->task_id;//FIXME:only use bootcpu
	size_t next_pid = last_task_pid+1;
	/*pick new task*/
	while(1)
	{
		if(next_pid >= NR_TASKS)
		{
			next_pid = 0;
		}
		if(tasks[next_pid].state == TASK_RUNNABLE)
		{
			//printk("next_pid=%d\n",next_pid);
			break;
		}
		next_pid++;
	}
	/*assign new task*///FIXME
	bootcpu->cpu_task = &tasks[next_pid];
	bootcpu->cpu_task->state = TASK_RUNNING;
	bootcpu->cpu_task->remind_ticks = TIME_QUANT;
	lcr3(PADDR(bootcpu->cpu_task->pgdir));

	/*record current task*/
	last_task_pid = next_pid;

	/*dispatch task*///FIXME
	ctx_switch(bootcpu->cpu_task);
}
