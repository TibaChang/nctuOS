/* Reference: http://www.osdever.net/bkerndev/Docs/pit.htm */
#include <kernel/trap.h>
#include <kernel/picirq.h>
#include <kernel/task.h>
#include <kernel/cpu.h>
#include <inc/mmu.h>
#include <inc/x86.h>

#define TIME_HZ 100

static unsigned long jiffies = 0;

void set_timer(int hz)
{
  int divisor = 1193180 / hz;       /* Calculate our divisor */
  outb(0x43, 0x36);             /* Set our command byte 0x36 */
  outb(0x40, divisor & 0xFF);   /* Set low byte of divisor */
  outb(0x40, divisor >> 8);     /* Set high byte of divisor */
}

/* It is timer interrupt handler */
//
// TODO: Lab6
// Modify your timer_handler to support Multi processor
// Don't forget to acknowledge the interrupt using lapic_eoi()
//
void timer_handler(struct Trapframe *tf)
{
  extern void sched_yield();
  extern Task tasks[];
  int i;
  jiffies++;
  lapic_eoi();


  /* 
   * TODO: Lab 5
   * 1. Maintain the status of slept tasks
   * 
   * 2. Change the state of the task if needed
   *
   * 3. Maintain the time quantum of the current task
   *
   * 4. sched_yield() if the time is up for current task
   *
   *
   */
	for(i = 0;i < NR_TASKS;i++)
	{
		if(thiscpu->cpu_rq.task_rq[i] != NULL)
		{
			switch( thiscpu->cpu_rq.task_rq[i]->state )
			{
				case TASK_SLEEP:
					thiscpu->cpu_rq.task_rq[i]->remind_ticks--;
					if(thiscpu->cpu_rq.task_rq[i]->remind_ticks<=0)
					{
						thiscpu->cpu_rq.task_rq[i]->state = TASK_RUNNABLE;
						thiscpu->cpu_rq.task_rq[i]->remind_ticks = TIME_QUANT;
					}
					break;
				case TASK_RUNNING:
					thiscpu->cpu_rq.task_rq[i]->remind_ticks --;
					break;
			}
		}
	}
	if(thiscpu->cpu_task->remind_ticks<=0)
	{
		sched_yield();
	}
}

unsigned long sys_get_ticks()
{
  return jiffies;
}
void timer_init()
{
  set_timer(TIME_HZ);

  /* Enable interrupt */
  irq_setmask_8259A(irq_mask_8259A & ~(1<<IRQ_TIMER));

  /* Register trap handler */
  extern void TIM_ISR();
  register_handler( IRQ_OFFSET + IRQ_TIMER, &timer_handler, &TIM_ISR, 0, 0);
}

