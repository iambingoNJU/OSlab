#include "common.h"
#include "x86.h"
#include "pcb.h"

void idle() {
	init_pcb_list();

	PCB *pcb = alloc_pcb();
	pcb->pid = alloc_pid();
	strcpy(pcb->name, "idle");
	pcb->state = RUNNING;
	pcb->time_count = TIME_SLICE;
	pcb->sleep_time = 0;
	pcb->sem = NULL;
	add_pcb(pcb);
	current_process = pcb;

	while(1) {
		wait_for_interrupt();
		printk("^");
		//printk("current process: %s\n", current_process->name);
		//show_all_process();
	}
}
