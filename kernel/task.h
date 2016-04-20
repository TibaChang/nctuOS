#ifndef TASK_H
#define TASK_H

#include <inc/trap.h>
#include <kernel/mem.h>
#define NR_TASKS	10
#define TIME_QUANT	100

typedef enum
{
	TASK_FREE = 0,
	TASK_RUNNABLE,
	TASK_RUNNING,
	TASK_SLEEP,
	TASK_STOP,
} TaskState;

// Each task's user space
#define USR_STACK_SIZE	(40960)

typedef struct
{
	int task_id;
	int parent_id;
	struct Trapframe tf; //Saved registers
	int32_t remind_ticks;
	TaskState state;	//Task state
  pde_t *pgdir;  //Per process Page Directory
	
} Task;

void task_init();
void env_pop_tf(struct Trapframe *tf);

/* TODO Lab 5
 * Interface for real implementation of kill and fork
 * Since their real implementation should be in kernel/task.c
 */

#endif
