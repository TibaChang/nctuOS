#include <kernel/task.h>
#include <kernel/cpu.h>
#include <inc/x86.h>

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
	extern struct CpuInfo cpus[NCPU];
	int f = cpunum();
	int flag;
	flag = 0;
	int i;
	int j=0;
	i = (cpus[f].cpu_rq.index + 1) % NR_TASKS;
	for(j=0;j<NR_TASKS;j++)
	{
		if(cpus[f].cpu_rq.task_rq[i]!=NULL && cpus[f].cpu_rq.task_rq[i]->state == TASK_RUNNABLE)
		{
			if(cpus[f].cpu_task->state == TASK_RUNNING  )
			{
				cpus[f].cpu_task->state = TASK_RUNNABLE;
				cpus[f].cpu_task->remind_ticks = TIME_QUANT;
				cpus[f].cpu_task = cpus[f].cpu_rq.task_rq[i];
				lcr3( PADDR( cpus[f].cpu_rq.task_rq[i]->pgdir ) );
				cpus[f].cpu_rq.task_rq[i]->state = TASK_RUNNING;
				cpus[f].cpu_rq.index = i;
				break;
				
			}
			
			if(cpus[f].cpu_task->state == TASK_SLEEP)
			{
				cpus[f].cpu_task = cpus[f].cpu_rq.task_rq[i];		
				lcr3( PADDR( cpus[f].cpu_rq.task_rq[i]->pgdir ) );
				cpus[f].cpu_rq.task_rq[i]->state = TASK_RUNNING;
				cpus[f].cpu_rq.index = i;
				break;
			}
			
			if(cpus[f].cpu_task->state == TASK_STOP)
			{
				cpus[f].cpu_task = cpus[f].cpu_rq.task_rq[i];
				lcr3( PADDR( cpus[f].cpu_rq.task_rq[i]->pgdir ) );
                                cpus[f].cpu_rq.task_rq[i]->state = TASK_RUNNING;
				cpus[f].cpu_rq.index = i;
                                break;
			}
			if(cpus[f].cpu_task->state == TASK_FREE)
			{
				cpus[f].cpu_task = cpus[f].cpu_rq.task_rq[i];
				lcr3( PADDR( cpus[f].cpu_rq.task_rq[i]->pgdir ) );
                                cpus[f].cpu_rq.task_rq[i]->state = TASK_RUNNING;
				cpus[f].cpu_rq.index = i;
                                break;
				
			}
		}
		else if(cpus[f].cpu_rq.task_rq[i]!=NULL && cpus[f].cpu_rq.task_rq[i]->state == TASK_RUNNING)
		{
			cpus[f].cpu_task = cpus[f].cpu_rq.task_rq[i];
			lcr3( PADDR( cpus[f].cpu_rq.task_rq[i]->pgdir ) );
			cpus[f].cpu_rq.task_rq[i]->remind_ticks = TIME_QUANT;	
		}
		i = (i+1)%NR_TASKS;


	}
	env_pop_tf(&cpus[f].cpu_task->tf);
	
}
