#include <inc/mmu.h>
#include <inc/types.h>
#include <inc/string.h>
#include <inc/x86.h>
#include <inc/memlayout.h>
#include <kernel/task.h>
#include <kernel/mem.h>
#include <kernel/cpu.h>
#include <kernel/spinlock.h>


struct spinlock task_lock;


// Global descriptor table.
//
// Set up global descriptor table (GDT) with separate segments for
// kernel mode and user mode.  Segments serve many purposes on the x86.
// We don't use any of their memory-mapping capabilities, but we need
// them to switch privilege levels. 
//
// The kernel and user segments are identical except for the DPL.
// To load the SS register, the CPL must equal the DPL.  Thus,
// we must duplicate the segments for the user and the kernel.
//
// In particular, the last argument to the SEG macro used in the
// definition of gdt specifies the Descriptor Privilege Level (DPL)
// of that descriptor: 0 for kernel and 3 for user.
//
struct Segdesc gdt[NCPU + 5] =
{
	// 0x0 - unused (always faults -- for trapping NULL far pointers)
	SEG_NULL,

	// 0x8 - kernel code segment
	[GD_KT >> 3] = SEG(STA_X | STA_R, 0x0, 0xffffffff, 0),

	// 0x10 - kernel data segment
	[GD_KD >> 3] = SEG(STA_W, 0x0, 0xffffffff, 0),

	// 0x18 - user code segment
	[GD_UT >> 3] = SEG(STA_X | STA_R, 0x0, 0xffffffff, 3),

	// 0x20 - user data segment
	[GD_UD >> 3] = SEG(STA_W , 0x0, 0xffffffff, 3),

	// First TSS descriptors (starting from GD_TSS0) are initialized
	// in task_init()
	[GD_TSS0 >> 3] = SEG_NULL

};

struct Pseudodesc gdt_pd = {
	sizeof(gdt) - 1, (unsigned long) gdt
};



static struct tss_struct tss;
Task tasks[NR_TASKS];

extern char bootstack[];

extern char UTEXT_start[], UTEXT_end[];
extern char UDATA_start[], UDATA_end[];
extern char UBSS_start[], UBSS_end[];
extern char URODATA_start[], URODATA_end[];
/* Initialized by task_init */
uint32_t UTEXT_SZ;
uint32_t UDATA_SZ;
uint32_t UBSS_SZ;
uint32_t URODATA_SZ;

Task *cur_task = NULL; //Current running task

extern void sched_yield(void);


/* TODO: Lab5
 * 1. Find a free task structure for the new task,
 *    the global task list is in the array "tasks".
 *    You should find task that is in the state "TASK_FREE"
 *    If cannot find one, return -1.
 *
 * 2. Setup the page directory for the new task
 *
 * 3. Setup the user stack for the new task, you can use
 *    page_alloc() and page_insert(), noted that the va
 *    of user stack is started at USTACKTOP and grows down
 *    to USR_STACK_SIZE, remember that the permission of
 *    those pages should include PTE_U
 *
 * 4. Setup the Trapframe for the new task
 *    We've done this for you, please make sure you
 *    understand the code.
 *
 * 5. Setup the task related data structure
 *    You should fill in task_id, state, parent_id,
 *    and its schedule time quantum (remind_ticks).
 *
 * 6. Return the pid of the newly created task.
 
 */
struct spinlock TASK_LOCK;
int task_create()
{
	Task *ts = NULL;
	struct PageInfo *pp;
	/* Find a free task structure */
	int pid;
	spin_lock(&TASK_LOCK);
	for(pid = 0; pid < NR_TASKS; pid++)
	{
		if(tasks[pid].state == TASK_FREE)
		{
			ts = &(tasks[pid]);
			break;
		}
	}
	spin_unlock(&TASK_LOCK);
	if(ts == NULL)
	{
		return -1;
	}
	/* Setup Page Directory and pages for kernel*/
	if (!(ts->pgdir = setupkvm()))
		panic("Not enough memory for per process page directory!\n");

	/* Setup User Stack */
	void *usr_stk;
	for(usr_stk = USTACKTOP-USR_STACK_SIZE;usr_stk < USTACKTOP;usr_stk = usr_stk + PGSIZE)
	{
		pp = page_alloc(ALLOC_ZERO);
		page_insert(ts->pgdir, pp, usr_stk,PTE_U | PTE_P | PTE_W);
	}
	/* Setup Trapframe */
	memset( &(ts->tf), 0, sizeof(ts->tf));

	ts->tf.tf_cs = GD_UT | 0x03;
	ts->tf.tf_ds = GD_UD | 0x03;
	ts->tf.tf_es = GD_UD | 0x03;
	ts->tf.tf_ss = GD_UD | 0x03;
	ts->tf.tf_esp = USTACKTOP-PGSIZE;

	/* Setup task structure (task_id and parent_id) */
	ts->task_id = pid;
	ts->state = TASK_RUNNABLE;
	if(thiscpu->cpu_task==NULL)
	{
		ts->parent_id = 0;
	}
	else
	{
		ts->parent_id = thiscpu->cpu_task->task_id;
	}
	ts->remind_ticks = TIME_QUANT;
	return pid;
}


/* TODO: Lab5
 * This function free the memory allocated by kernel.
 *
 * 1. Be sure to change the page directory to kernel's page
 *    directory to avoid page fault when removing the page
 *    table entry.
 *    You can change the current directory with lcr3 provided
 *    in inc/x86.h
 *
 * 2. You have to remove pages of USER STACK
 *
 * 3. You have to remove pages of page table
 *
 * 4. You have to remove pages of page directory
 *
 * HINT: You can refer to page_remove, ptable_remove, and pgdir_remove
 */

static void task_free(int pid)
{
	lcr3(PADDR(kern_pgdir));
	void* usr_stk;
	for(usr_stk = USTACKTOP-USR_STACK_SIZE;usr_stk < USTACKTOP;usr_stk = usr_stk+PGSIZE)
	{
		page_remove(tasks[pid].pgdir,usr_stk);
	}
	ptable_remove(tasks[pid].pgdir);
	pgdir_remove(tasks[pid].pgdir);
}

// Lab6 TODO
//
// Modify it so that the task will be removed form cpu runqueue
// ( we not implement signal yet so do not try to kill process
// running on other cpu )
//
void sys_kill(int pid)
{
	if (pid > 0 && pid < NR_TASKS)
	{
		/* TODO: Lab 5
		 * Remember to change the state of tasks
		 * Free the memory
		 * and invoke the scheduler for yield
		 */
		int rq_idx;
		for(rq_idx=0;rq_idx<NR_TASKS;rq_idx++)
		{
			if(thiscpu->cpu_rq.task_rq[rq_idx]!=NULL && thiscpu->cpu_rq.task_rq[rq_idx]->task_id == pid)
			{
				task_free(pid);
				tasks[pid].state = TASK_FREE;
				thiscpu->cpu_rq.number--;
				sched_yield();
			}
		}
	}
}

/* TODO: Lab 5
 * In this function, you have several things todo
 *
 * 1. Use task_create() to create an empty task, return -1
 *    if cannot create a new one.
 *
 * 2. Copy the trap frame of the parent to the child
 *
 * 3. Copy the content of the old stack to the new one,
 *    you can use memcpy to do the job. Remember all the
 *    address you use should be virtual address.
 *
 * 4. Setup virtual memory mapping of the user prgram 
 *    in the new task's page table.
 *    According to linker script, you can determine where
 *    is the user program. We've done this part for you,
 *    but you should understand how it works.
 *
 * 5. The very important step is to let child and 
 *    parent be distinguishable!
 *
 * HINT: You should understand how system call return
 * it's return value.
 */

//
// Lab6 TODO:
//
// Modify it so that the task will disptach to different cpu runqueue
// (please try to load balance, don't put all task into one cpu)
//
int sys_fork()
{
	/* pid for newly created process */
	int pid;
	pid = task_create();
	if(pid<0)
	{
		return -1;
	}

	uintptr_t usr_stk;
	int f = cpunum();
    memcpy(&(tasks[pid].tf) , &(cpus[f].cpu_task->tf),sizeof(struct Trapframe));
    for( usr_stk = (USTACKTOP-USR_STACK_SIZE); usr_stk < USTACKTOP; usr_stk+= PGSIZE)
    {
        struct PageInfo* a = page_lookup( tasks[pid].pgdir , (void *)usr_stk ,NULL );
        memcpy( page2kva(a) , usr_stk ,PGSIZE);
    }
                        
	
	/* Step 4: All user program use the same code for now */
	setupvm(tasks[pid].pgdir, (uint32_t)UTEXT_start, UTEXT_SZ);
	setupvm(tasks[pid].pgdir, (uint32_t)UDATA_start, UDATA_SZ);
	setupvm(tasks[pid].pgdir, (uint32_t)UBSS_start, UBSS_SZ);
	setupvm(tasks[pid].pgdir, (uint32_t)URODATA_start, URODATA_SZ);

	/*assign to cpu with loading balance*/
	int min = bootcpu->cpu_rq.number;
	int min_id;
	int i;
	for(i=0;i<ncpu;i++)
	{	
		if(min >= cpus[i].cpu_rq.number)
		{
			min = cpus[i].cpu_rq.number;
			min_id = cpus[i].cpu_id;
		}
	}
	int t_index = cpus[min_id].cpu_rq.index;
	int t_number = cpus[min_id].cpu_rq.number;
	cpus[min_id].cpu_rq.task_rq[(t_index + t_number)%NR_TASKS] = &tasks[pid];
	cpus[min_id].cpu_rq.number++;
	tasks[pid].tf.tf_regs.reg_eax = 0;
	return pid;
}

/* TODO: Lab5
 * We've done the initialization for you,
 * please make sure you understand the code.
 */
void task_init()
{
	extern int user_entry();
	int i;
	UTEXT_SZ = (uint32_t)(UTEXT_end - UTEXT_start);
	UDATA_SZ = (uint32_t)(UDATA_end - UDATA_start);
	UBSS_SZ = (uint32_t)(UBSS_end - UBSS_start);
	URODATA_SZ = (uint32_t)(URODATA_end - URODATA_start);

	/* Initial task sturcture */
	for (i = 0; i < NR_TASKS; i++)
	{
		memset(&(tasks[i]), 0, sizeof(Task));
		tasks[i].state = TASK_FREE;
	}
	task_init_percpu();
}

// Lab6 TODO
//
// Please modify this function to:
//
// 1. init idle task for non-booting AP 
//    (remember to put the task in cpu runqueue) 
//
// 2. init per-CPU Runqueue
//
// 3. init per-CPU system registers
//
// 4. init per-CPU TSS
//
void task_init_percpu()
{
	int i;
	extern int user_entry();
	extern int idle_entry();
	int f;
	
	f = cpunum();	
	// Setup a TSS so that we get the right stack
	// when we trap to the kernel.
	memset(&(thiscpu->cpu_tss),0,sizeof(thiscpu->cpu_tss));
	thiscpu->cpu_tss.ts_esp0 = percpu_kstacks[f] + KSTKSIZE;
	thiscpu->cpu_tss.ts_ss0 = GD_KD;

	int j;
	for(j=0;j<NR_TASKS;j++)
	{
		thiscpu->cpu_rq.task_rq[j] = NULL;
	}

	// fs and gs stay in user data segment
	thiscpu->cpu_tss.ts_fs = GD_UD | 0x03;
	thiscpu->cpu_tss.ts_gs = GD_UD | 0x03;

	/* Setup TSS in GDT */
	gdt[(GD_TSS0  >> 3) + f] = SEG16(STS_T32A, (uint32_t)(&(cpus[f].cpu_tss)), sizeof(struct tss_struct) , 0);
	gdt[(GD_TSS0  >> 3) + f ].sd_s = 0;
	
	/* Setup first task */
	spin_lock(&task_lock);
	i = task_create();
	spin_unlock(&task_lock);
	thiscpu->cpu_task = &(tasks[i]);
	
	/* For user program */
	setupvm(thiscpu->cpu_task->pgdir, (uint32_t)UTEXT_start, UTEXT_SZ);
	setupvm(thiscpu->cpu_task->pgdir, (uint32_t)UDATA_start, UDATA_SZ);
	setupvm(thiscpu->cpu_task->pgdir, (uint32_t)UBSS_start, UBSS_SZ);
	setupvm(thiscpu->cpu_task->pgdir, (uint32_t)URODATA_start, URODATA_SZ);
	if(thiscpu != bootcpu)
	{
		spin_lock(&task_lock);
		thiscpu->cpu_task->tf.tf_eip = (uint32_t)idle_entry;
		spin_unlock(&task_lock);
	}
	else
	{
		spin_lock(&task_lock);
		thiscpu->cpu_task->tf.tf_eip = (uint32_t)user_entry;
		spin_unlock(&task_lock);
	}
	thiscpu->cpu_rq.number = 1;
	thiscpu->cpu_rq.index = 0;
	thiscpu->cpu_rq.task_rq[0] = thiscpu->cpu_task;
	thiscpu->cpu_task->state = TASK_RUNNING;

	/* Load GDT&LDT */
	lgdt(&gdt_pd);
	lldt(0);
	ltr((GD_TSS0 & 0x3) | (((GD_TSS0 >> 3) + thiscpu->cpu_id) << 3) );
}
