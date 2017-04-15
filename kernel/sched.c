#include <kernel/task.h>
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
void sched_yield(void)
{
	extern Task tasks[];
	extern Task *cur_task;
	last_task_pid = cur_task->task_id;
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
	/*assign new task*/
	cur_task = &tasks[next_pid];
	cur_task->state = TASK_RUNNING;
	cur_task->remind_ticks = TIME_QUANT;
	lcr3(PADDR(cur_task->pgdir));

	/*record current task*/
	last_task_pid = next_pid;

	/*dispatch task*/
	ctx_switch(cur_task);
}
