#include "common.h"
#include "x86.h"
#include "device.h"
#include "pcb.h"
#include "../../lib/syscall.h"

void load_first_process(PCB* pcb);

void do_syscall(struct TrapFrame *tf)
{
	switch(tf->eax)
	{
		case SYS_puts:
		{
			char *s = (char*)tf->edx;
			uint32_t len = tf->ecx;
			while(len--)	putchar(*s++);
			tf->eax = tf->ecx;
			break;
		}
		case SYS_pcb_table:
		{
			show_all_process();
			break;
		}
		case SYS_fork:
		{
			//printk("syscall: fork()\n");
			PCB *newpcb = alloc_pcb();
			newpcb->pid = alloc_pid();

			//Copy registers
			//Remember this is not a completely copy!!!!!
			memcpy((void*)(&(newpcb->tf)), (void*)(&(current_process->tf)), sizeof(struct TrapFrame));
			newpcb->tf.eflags |= 0x200;		//interrupt enable
			newpcb->tf.ebp = (uint32_t)newpcb->stack - (uint32_t)current_process->stack + current_process->tf.ebp;
			newpcb->tf.esp = (uint32_t)newpcb->stack - (uint32_t)current_process->stack + current_process->tf.esp;

			//Copy stack
			memcpy(newpcb->stack, current_process->stack, PROCESS_STACK_SIZE);
			//printk("before stack copy\n");
			uint32_t temp;
			temp = current_process->tf.ebp;
			while(temp != ((uint32_t)current_process->stack + PROCESS_STACK_SIZE)) {
				*((uint32_t*)((uint32_t)newpcb->stack + temp - (uint32_t)current_process->stack)) = 
					*((uint32_t*)temp) - (uint32_t)current_process->stack + (uint32_t)newpcb->stack;
				temp = *((uint32_t*)temp);
			}
			//printk("after stack copy\n");

			//Other PCB fields
			memcpy(newpcb->name, current_process->name, 32);
			newpcb->state = RUNNABLE;
			newpcb->time_count = current_process->time_count / 2;
			current_process->time_count /= 2; 
			newpcb->sleep_time = current_process->sleep_time;
			newpcb->sem = current_process->sem;
			add_pcb(newpcb);

			//Set return value
			current_process->tf.eax = newpcb->pid;	//Parent process return pid of child.
			current_process->tf.eflags |= 0x200;
			schedule(RUNNABLE);
			current_process->tf.eax = 0;	//Child process return 0.
			break;
		}
		case SYS_sleep:
		{
			//printk("syscall: sleep()\n");
			current_process->sleep_time = tf->edx;
			schedule(tf->edx ? BLOCKED : RUNNABLE);
			break;
		}
		case SYS_exit:
		{
			//printk("syscall: exit()\n");
			schedule(DEAD);
			break;
		}
		case SYS_createSem:
		{
			if(current_process->sem != NULL) {
				printk("Semaphore already existed!\n");
				assert(0);
			} else {
				init_sem(0);
				current_process->sem = &sem;
			}
			break;
		}
		case SYS_lockSem:
		{
			current_process->sem->P(current_process->sem);
			break;
		}
		case SYS_unlockSem:
		{
			current_process->sem->V(current_process->sem);
			break;
		}
		case SYS_destroySem:
		{
			current_process->sem = NULL;
			break;
		}

		default: 
			printk("Unhandled syscall: #%d\n", tf->eax);
			assert(0);
	}
}

void
irq_handle(struct TrapFrame *tf) {
    /*
     * 中断处理程序
     */
	int irq = tf->irq;

	current_process->tf.gs = tf->gs;
	current_process->tf.fs = tf->fs;
	current_process->tf.es = tf->es;
	current_process->tf.ds = tf->ds;
	current_process->tf.edi = tf->edi;
	current_process->tf.esi = tf->esi;
	current_process->tf.ebp = tf->ebp;
	current_process->tf.xxx = tf->xxx;
	current_process->tf.ebx = tf->ebx;
	current_process->tf.edx = tf->edx;
	current_process->tf.ecx = tf->ecx;
	current_process->tf.eax = tf->eax;
	current_process->tf.irq = tf->irq;
	current_process->tf.eip = tf->eip;
	current_process->tf.cs = tf->cs;
	current_process->tf.eflags = tf->eflags;
	current_process->tf.esp = tf->esp;
	current_process->tf.ss = tf->ss;

	if(irq < 0) {
		printk("Unhandled exception!\n");
		assert(0);
	} else if(irq == 0x80) {
		do_syscall(tf);
	} else if(irq < 1000) {
		printk("Unexcepted exception #%d\n", irq);
	} else if(irq >= 1000) {
		if(irq == 1000) {
			//printk("timer interrupt!\n");
			printk("");
			//show_all_process();
			blocked_process_handler();
			if(current_process->pid == 0) {
				schedule(BLOCKED);
			}
			if(nr_process == 1)  {	//first time for user program to execute.
				PCB *pcb = alloc_pcb();
				load_first_process(pcb);
				add_pcb(pcb);			
				current_process->state = BLOCKED;
				current_process = pcb;
				current_process->state = RUNNING;
			} else if(current_process->time_count > 0) {
				current_process->time_count--;
				//printk("time_count: %d(pid = %d)\n", current_process->time_count, current_process->pid);
			} else if(current_process->time_count == 0) {
				current_process->time_count = TIME_SLICE;
				schedule(RUNNABLE);
			}
		}
		else
			printk("Unhandled IRQ exception #%d\n", irq);
	}
}

